#include "matchManager.hpp"

///Private function signatures intended only for use in current file
void validateSend(int sendRetVal, int expectedByteCount);
///end private signatures

//std::mutex m; //refactored the globals out
//std::condition_variable cv;

int main(int argc, char **argv) {
    struct GameState gameState = {
        .gameOver = false,
        .whiteReady = false,
        .blackReady = false,
        .timeUp = false,
        .blackTime = 60*5*1000,
        .whiteTime = 60*5*1000 //default time in ms: 5 minutes
    };
    std::string fen = STARTFEN;
    for (int i = 0; i < argc; ++i) {
        std::string s = std::string(argv[i]);
        if (s == "-wt") {
            gameState.whiteTime = std::atoi(argv[i+1]);
        } 
        else if (s == "-bt") {
            gameState.whiteTime = std::atoi(argv[i+1]);
        } 
        else if (s == "-f") {
            fen = std::string(argv[i+1]);
            int j = i + 2;
            //Finds new arg or reaches the end
            //Fens can have dashes so we need a better way of dealing with this when we add new args
            while (
                std::string(argv[j]).find("-wt") == std::string::npos &&
                std::string(argv[j]).find("-bt") == std::string::npos &&
                j < argc
            ) {
                fen += " ";
                fen += std::string(argv[j]);
                j++;
            }
            break;
        } 
    }
    struct GameHistory gameHistory = {
        .moveIndex = 0,
        .startFen = fen,
    };

    std::cerr << "fen: " << fen << std::endl;

    //Match Manager needs to be a source of true for moves
    generateKingAttacks();
    generateKnightAttacks();

    std::thread userMatchManagerThread;
    std::thread userCLIThread;
    std::thread engineOneThread;
    std::thread engineTwoThread;
    std::string UCIResponse; //locked behind the mutex condition
    bool responseReady = false; //locked behind the mutex condition
    
    uint32_t lightColor = 0xffffffff;
    uint32_t darkColor = 0xff4a9627;
    fenToBoardState(fen, std::ref(gameState.state));
    gameState.turnState = gameState.state.sideToMove;


    userMatchManagerThread = std::thread(
        matchManagerThread,
        std::ref(gameState),
        std::ref(gameHistory),
        std::ref(responseReady),
        std::ref(UCIResponse)
    );

    userCLIThread = std::thread(
        CLIThread,
        std::ref(gameState.gameOver),
        std::ref(gameState.timeUp),
        std::ref(gameState.threadSyncMutex),
        std::ref(gameState.mutexCondition)        
    );
    engineOneThread = std::thread(
        engineThread,
        std::ref(gameState),
        std::ref(gameHistory),
        WHITE,
        std::ref(UCIResponse),
        std::ref(responseReady)
    );
    engineTwoThread = std::thread(
        engineThread,
        std::ref(gameState),
        std::ref(gameHistory),
        BLACK,
        std::ref(UCIResponse),
        std::ref(responseReady)
    );

    renderBoard(
        std::ref(gameState.state),
        std::ref(gameState.gameOver),
        darkColor,
        lightColor,
        "img",
        std::ref(gameState.threadSyncMutex)
    );

    //If game ends because of gui close
    gameState.gameOver = true;
    gameState.mutexCondition.notify_all();

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
    struct GameState& gameState,
    struct GameHistory& gameHistory,
    int color,
    std::string& UCIResponse,
    bool& responseReady
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
    int res = bind(sockDesc, (const struct sockaddr *)&listenAddressOne, sizeof(listenAddressOne));
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
        if (gameState.gameOver) { return; }
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
    if (color == WHITE) {
        gameState.whiteReady = true;
    }
    else {
        gameState.blackReady = true;
    }
    gameState.mutexCondition.notify_all();
    //Could output the contents of clientConnInfo for logging / connection debug
    std::cerr << "matchManager - Client successfully connected: "
        << ntohs(clientConnInfo.sin_addr.s_addr)
        << ":" << ntohs(clientConnInfo.sin_port) << std::endl;
    
    //std::cerr << "wready = " << gameState.whiteReady << " bready = " << gameState.blackReady << std::endl;
    std::string cmd = "uci\n";
    send(clientDesc, cmd.c_str(), cmd.length(), 0);
    char buf[PACKET_STR_SIZE]; 
    std::string cppBuf;
    do {
        res = recv(clientDesc, buf, PACKET_STR_SIZE, 0);
        cppBuf = std::string(buf);
    } while (cppBuf.find("id name") == std::string::npos);
    std::cerr << "{{{ " << buf << " }}}" << std::endl << std::endl;
    if (res > 0) {
        cppBuf = std::string(buf);
        std::istringstream ss(cppBuf);
        std::string token;
        while (ss >> token) {
            if (token == "name") {
                std::string engineName;
                std::getline(ss, engineName);

                if (color == WHITE) {
                    gameHistory.whiteName = engineName;
                }
                else {
                    gameHistory.blackName = engineName;
                }
            }
        }
    } else {
        std::cerr << "Failed to get info after uci command sent" << std::endl;
    }


    if (color == WHITE) {
        std::cerr << "white's name = " << gameHistory.whiteName << std::endl;
    }
    else {
        std::cerr << "black's name = " << gameHistory.blackName << std::endl;
    }

    cmd = "isready\n";
    send(clientDesc, cmd.c_str(), cmd.length(), 0);
    
    //draining all data from pending recv queue
    while (1) { 
        //char buf[PACKET_STR_SIZE]; 
        const int res = recv(clientDesc, buf, PACKET_STR_SIZE, MSG_DONTWAIT);
        if (res == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) { break; }
    }

    while (true) {
        std::unique_lock lk(gameState.threadSyncMutex);
        gameState.mutexCondition.wait(lk, [color, &gameState, &responseReady]{ 
                return (!responseReady && (color == gameState.turnState) && gameState.whiteReady && gameState.blackReady) || gameState.gameOver; 
        }); //my turn
        std::cerr << "matchManager engine thread woken up, color=" << color << std::endl;
        lk.unlock(); //This allows main thread to continue to act
        if(gameState.gameOver){
            char buf[] = "bye";
            send(clientDesc, (void *) buf, sizeof(buf), MSG_DONTWAIT);
            close(clientDesc); //this technically has a return value
                //to indicate failure, but it's dumb
            break;
        }

        lk.lock();
        cmd = createPositionCmd(gameState.state) + "\n";
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
        
        cmd = "go wtime " + std::to_string(gameState.whiteTime) + " btime " + std::to_string(gameState.blackTime) + "\n";
        std::strncpy(buf, cmd.c_str(), PACKET_STR_SIZE); 
        buf[PACKET_STR_SIZE - 1] = '\0';
        //send Go command
        std::cerr << "matchManager DEBUG2: transmitting {" << std::string(buf) << "}" << std::endl;
        bytesSent = send(clientDesc, (void *) buf, cmd.length(), 0);
        validateSend(bytesSent, cmd.length());
        //await engine response
        int bytesRead;
        std::memset((void *) buf, 0, PACKET_STR_SIZE); //clearing buffer for sanity's sake
        do {
            bytesRead = recv(clientDesc, buf, PACKET_STR_SIZE - 1, MSG_DONTWAIT);
            if (bytesRead == -1 && (errno == EWOULDBLOCK || errno == EAGAIN)) {
                std::this_thread::sleep_for(std::chrono::milliseconds(2));
                if (gameState.gameOver) {
                    char buf[] = "bye";
                    const int res = send(clientDesc, (void *) buf, sizeof(buf), 0);
                    std::cerr << "res = " << res << std::endl;
                    close(clientDesc); //this technically has a return value
                        //to indicate failure, but it's dumb
                    return;
                }
                else if (gameState.timeUp) {
                    char buf[] = "stop\n";
                    const int res = send(clientDesc, (void *) buf, sizeof(buf), 0);
                    if (res == -1) {
                        std::cerr << "Failed to send time up signal" << std::endl;
                    }
                    gameState.timeUp = false;
                    continue;
                }
                continue;
            }
            //TODO -- can later parse the other info the engines send in here
            //such as what the engine thought of a certain position, etc
        } while (std::string(buf).find("bestmove") == std::string::npos && !gameState.gameOver);
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
        gameState.mutexCondition.notify_all();
    }
    return;
}

void CLIThread(std::atomic<bool>& gameOver, std::atomic<bool>& timeUp, std::mutex& m, std::condition_variable& cv){
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
        else if (userInput == "stop") { //tells the current engine to immediately stop thinking and send its current best move
            timeUp = true;
        }
        else{
            std::cout << "Unknown Command: " << userInput << std::endl;
        }
    }
}

void matchManagerThread(
    struct GameState& gameState,
    struct GameHistory& gameHistory,
    bool& responseReady,
    std::string& UCIResponse
){

    //auto now = std::chrono::system_clock::now();
    std::time_t timeBeforeMove = 0;
    std::time_t timeAfterMove = 0;
    //std::chrono::milliseconds timeBeforeMove = std::chrono::milliseconds::zero();
    //std::chrono::milliseconds timeAfterMove = std::chrono::milliseconds::zero();
    while(true){
        std::unique_lock lk(gameState.threadSyncMutex);
        gameState.mutexCondition.wait(lk, [&gameState, &responseReady]{ return gameState.gameOver || responseReady;});
        if(gameState.gameOver){
            lk.unlock();
            //Dont need to notify because CLIThread already woke up other threads
            break;
        }
        //std::time is in seconds
        timeAfterMove = std::time(nullptr) * 1000;

        std::time_t timeDiff = timeAfterMove - timeBeforeMove;
        if (timeBeforeMove != 0) {
            if (gameState.turnState == WHITE) {
                gameState.whiteTime -= timeDiff;
            } else {
                gameState.blackTime -= timeDiff;
            }
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

            Move engineMove = strMoveToMove(token, gameState.state);

            std::cerr << "engineMove.src: " << engineMove.source << std::endl << "engineMove.dest: " << engineMove.dest << std::endl;

            UndoState undo;
            makeMove(gameState.state, engineMove, undo);
            std::cerr << "After makeMove call: " << boardStateToFen(gameState.state) << std::endl;
            UCIResponse = ""; //Clear UCIResponse
            responseReady = false;
            gameState.turnState = gameState.state.sideToMove;
            MoveList legalMoves = generateMoves(gameState.state, gameState.state.sideToMove);
            //Checks if game is over
            if(legalMoves.count == 0){
                gameState.gameOver = true;
                lk.unlock();
                gameState.mutexCondition.notify_all();
                break;
            }
            timeBeforeMove = std::time(nullptr) * 1000;
            lk.unlock();
            gameState.mutexCondition.notify_all();
        }
        else{
            lk.unlock();
        }
        lk.lock();
        responseReady = false;
        lk.unlock();
        gameState.mutexCondition.notify_all();
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
