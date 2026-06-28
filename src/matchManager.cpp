#include "matchManager.hpp"

///Private function signatures intended only for use in current file
void validateSend(int sendRetVal, int expectedByteCount);
///end private signatures

//std::mutex m; //refactored the globals out
//std::condition_variable cv;

int main(int argc, char **argv) {
    std::string fen = STARTFEN;
    for (int i = 0; i < argc; ++i) {
        std::string s = std::string(argv[i]);
        if (s == "-f") {
            fen = std::string(argv[i+1]);
            int j = i + 2;
            //Finds new arg or reaches the end
            //Fens can have dashes so we need a better way of dealing with this when we add new args
            while(/*std::string(argv[j]).find("-") == std::string::npos &&*/ j < argc){
                fen += " ";
                fen += std::string(argv[j]);
                j++;
            }
            break;
        } 
    }

    std::cerr << "fen: " << fen << std::endl;

    //Match Manager needs to be a source of true for moves
    generateKingAttacks();
    generateKnightAttacks();

    std::thread userMatchManagerThread;
    std::thread userCLIThread;
    std::thread engineOneThread;
    std::thread engineTwoThread;
    std::atomic<int> turnState;
    std::atomic<bool> gameOver = false;
    struct BoardState state;
    std::mutex threadSyncMutex;
    std::condition_variable mutexCondition;
    std::string UCIResponse; //locked behind the mutex condition
    bool responseReady = false; //locked behind the mutex condition
    
    fenToBoardState(fen, std::ref(state));
    turnState = state.sideToMove;

    uint32_t lightColor = 0xffffffff;
    uint32_t darkColor = 0xff4a9627;


    userMatchManagerThread = std::thread(
        matchManagerThread,
        std::ref(gameOver),
        std::ref(turnState),
        std::ref(responseReady),
        std::ref(state),
        std::ref(UCIResponse),
        std::ref(threadSyncMutex),
        std::ref(mutexCondition)        
    );

    userCLIThread = std::thread(
        CLIThread,
        std::ref(gameOver),
        std::ref(threadSyncMutex),
        std::ref(mutexCondition)        
    );
    engineOneThread = std::thread(
        engineThread,
        std::ref(turnState),
        WHITE,
        std::ref(gameOver),
        std::ref(state),
        std::ref(UCIResponse),
        std::ref(responseReady),
        std::ref(threadSyncMutex),
        std::ref(mutexCondition)
    );
    engineTwoThread = std::thread(
        engineThread,
        std::ref(turnState),
        BLACK,
        std::ref(gameOver),
        std::ref(state),
        std::ref(UCIResponse),
        std::ref(responseReady),
        std::ref(threadSyncMutex),
        std::ref(mutexCondition)
    );

    renderBoard(
        std::ref(state),
        std::ref(gameOver),
        darkColor,
        lightColor,
        "img",
        std::ref(threadSyncMutex)
    );

    //If game ends because of gui close
    gameOver = true;
    mutexCondition.notify_all();

    if(userMatchManagerThread.joinable()){
        userMatchManagerThread.join();
    }
    if(engineOneThread.joinable()){
        engineOneThread.join();
    }
    if(engineTwoThread.joinable()){
        engineTwoThread.join();
    }
    if(userCLIThread.joinable()){
        userCLIThread.join();
    }
    //I think it's fine to join now, UCIThread can't block infinitely anymore.
    //It was actually the engine threads blocking.
    //~~Since UCI thread can block forever on std::in we can't ensure that it will be joinable
    //~~Let the OS handle it
    //userCLIThread.detach();

}

void engineThread(
    const std::atomic<int>& turnState,
    int color,
    std::atomic<bool>& gameOver,
    BoardState& state,
    std::string& UCIResponse,
    bool& responseReady,
    std::mutex& m,
    std::condition_variable& cv
) {
    int sockDesc = -1;
    if ((sockDesc = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0)) == -1) {
        std::cerr << "matchManager - Failed to create socket in engineThread: color = " << color << std::endl;
    }
    struct sockaddr_in listenAddressOne = {
        .sin_family = AF_INET,
        .sin_port = htons(ENGINE_LISTEN_PORT[color]), //Color *must* be 0 or 1 (white, black)
        .sin_addr = {.s_addr = INADDR_ANY} //(man 7 ip)
    };
    const int reuse = 1;
    setsockopt(sockDesc, SOL_SOCKET, SO_REUSEADDR, (void *) &reuse, sizeof(reuse));
    const int res = bind(sockDesc, (const struct sockaddr *)&listenAddressOne, sizeof(listenAddressOne));
    if (res == -1) {
        std::cerr << "matchManager - Failed to bind socket. Errno=" << errno << std::endl;
        return;
    }

    const int connectionBacklogLimit = 1;
    listen(sockDesc, connectionBacklogLimit);
    struct sockaddr_in clientConnInfo; //filled in with call to accept()
    socklen_t connSizeInfo = sizeof(clientConnInfo); //will be overwritten by accept()
    //At some point we probably want to make this a non-blocking loop
    //because we want to be able to abort without waiting for engines to connect
    //-- To abort, we should probably be checking against some abort flag in the do-while, no?
    //I'll leave the choice of how to go about that undecided for now.
    int clientDesc = -1;
    do {
        if (gameOver) { return; }
        clientDesc = accept(sockDesc, (struct sockaddr *) &clientConnInfo, &connSizeInfo);
        if (clientDesc == -1) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                continue;
            } else {
                std::cerr << "matchManager - Engine Thread (color=" << color << ") failed accept(), errno=" << errno << std::endl;
                return; //Probably should gracefully close down the threads and such at some point
                //For now, just returning early. TODO
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100)); //TODO - magic number
    } while (clientDesc == -1);
    int flags = fcntl(clientDesc, F_GETFL);
    flags &= ~(O_NONBLOCK);
    fcntl(clientDesc, F_SETFL, flags);
    //Could output the contents of clientConnInfo for logging / connection debug
    std::cerr << "matchManager - Client successfully connected: "
        << ntohs(clientConnInfo.sin_addr.s_addr)
        << ":" << ntohs(clientConnInfo.sin_port) << std::endl;
    
    while (true) {
        std::unique_lock lk(m);
        cv.wait(lk, [color, &turnState, &gameOver, &responseReady]{ 
                return (!responseReady && color == turnState) || gameOver; 
        }); //my turn
        std::cerr << "matchManager engine thread woken up, color=" << color << std::endl;
        lk.unlock(); //This allows main thread to continue to act
        if(gameOver){
            char buf[] = "bye";
            send(clientDesc, (void *) buf, sizeof(buf), MSG_DONTWAIT);
            close(clientDesc); //this technically has a return value
                //to indicate failure, but it's dumb
            break;
        }

        lk.lock();
        std::string cmd = createPositionCmd(state) + "\n";
        lk.unlock();
        assert(cmd.length() <= PACKET_STR_SIZE); //Need some form of bounds checking
            //better to crash than error silently

        char buf[PACKET_STR_SIZE] = {0};
        std::strncpy(buf, cmd.c_str(), PACKET_STR_SIZE);
        buf[PACKET_STR_SIZE - 1] = '\0';
        //send Position command
        std::cerr << "matchManager DEBUG: transmitting {" << std::string(buf) << "}" << std::endl;
        int bytesSent = send(clientDesc, (void *) buf, cmd.length(), MSG_MORE);
        validateSend(bytesSent, cmd.length());
        
        cmd = "go\n";
        std::strncpy(buf, cmd.c_str(), PACKET_STR_SIZE); 
        buf[PACKET_STR_SIZE - 1] = '\0';
        //send Go command
        std::cerr << "matchManager DEBUG2: transmitting {" << std::string(buf) << "}" << std::endl;
        bytesSent = send(clientDesc, (void *) buf, cmd.length(), 0);
        validateSend(bytesSent, cmd.length());
        //await engine response
        int bytesRead;
        do {
            if (gameOver) { close(clientDesc); return; }
            bytesRead = recv(clientDesc, buf, PACKET_STR_SIZE - 1, 0);
            //TODO -- can later parse the other info the engines send in here
            //such as what the engine thought of a certain position, etc
        } while (std::string(buf).find("bestmove") == std::string::npos && !gameOver);
        if(bytesRead == 0){
            std::cerr << "bytesRead was zero in matchManager" << std::endl;
            close(clientDesc);
            return;
        } else if (bytesRead == -1) {
            std::cerr << "matchManager recv call failed with errno=" << errno << std::endl;
            close(clientDesc);
            return;
        }
        //ensure std::string cast wont pickup garbage
        buf[bytesRead] = '\0';
        std::cerr << buf << std::endl;
        lk.lock(); //This ensure that it is safe to write to the global UCIResponse
                   //Would it make sense to just make UCIResponse non-global, and passed
                   //as an atomic value to the threads that need to access it?
        //send received move to main thread
        UCIResponse = std::string(buf);
        std::cerr << "UCIResponse from thread (color=" << color << "):" << UCIResponse << std::endl;
        responseReady = true;
        lk.unlock();
        cv.notify_all();
    }

    return;
}

void CLIThread(std::atomic<bool>& gameOver, std::mutex& m, std::condition_variable& cv){
    while(true){
        std::string userInput;
        std::cin >> userInput;
        std::cout << userInput;
        if(userInput == "abort" || std::cin.eof() || gameOver == true){
            std::unique_lock lk(m);
            gameOver = true;
            lk.unlock();
            cv.notify_all();
            break;
        }
        else{
            std::cout << "Unknown Command: " << userInput << std::endl;
        }
    }
}

void matchManagerThread(
    std::atomic<bool>& gameOver,
    std::atomic<int>& turnState,
    bool& responseReady,
    BoardState& state,
    std::string& UCIResponse,
    std::mutex& threadSyncMutex,
    std::condition_variable& mutexCondition    
){
    while(true){
        std::unique_lock lk(threadSyncMutex);
        mutexCondition.wait(lk, [&gameOver, &responseReady]{ return gameOver || responseReady;});
        if(gameOver){
            lk.unlock();
            //Dont need to notify because CLIThread already woke up other threads
            break;
        }
        if(UCIResponse.find("bestmove") != std::string::npos){
            std::istringstream ss(UCIResponse);
            std::string token;
            //Skip unti we reach "bestmove"
            //Not the cleanest fix had to add this to deal with the engine thread sending: 
            //"info score cp 0bestmove b1c3"
            //Should probably figure out why the engine is sending info and bestmove together
            do{
                ss >> token;
            } while(token.find("bestmove") == std::string::npos);
            //token equals move ie "e2e4"
            ss >> token;

            std::cerr << "move token: " << token << std::endl;

            Move engineMove = strMoveToMove(token, state);

            std::cerr << "engineMove.src: " << engineMove.source << std::endl << "engineMove.dest: " << engineMove.dest << std::endl;

            UndoState undo;
            makeMove(state, engineMove, undo);
            std::cerr << "After makeMove call: " << boardStateToFen(state) << std::endl;
            UCIResponse = ""; //Clear UCIResponse
            responseReady = false;
            turnState = state.sideToMove;
            MoveList legalMoves = generateMoves(state, state.sideToMove);
            //Checks if game is over
            if(legalMoves.count == 0){
                gameOver = true;
                lk.unlock();
                mutexCondition.notify_all();
                break;
            }
            lk.unlock();
            mutexCondition.notify_all();
        }
        else{
            lk.unlock();
        }
        lk.lock();
        responseReady = false;
        lk.unlock();
        mutexCondition.notify_all();
    }
}

///intended for match manager internal use only
void validateSend(int sendRetVal, int expectedByteCount) {
    if (sendRetVal == -1) {
        std::cerr << "DEBUG ERROR: matchManager - failed to send cmd string. errno="
            << errno << std::endl;
        return;
        //TODO - probably should have some sort of error function that shuts down
        //all the components of the program cleanly, and can be called from anywhere.
    } else if (sendRetVal != expectedByteCount) {
        std::cerr << "DEBUG INFO: matchManager - socket bytes sent isn't expected value"
            << "continuing execution..." << std::endl;
    } else { }
}
