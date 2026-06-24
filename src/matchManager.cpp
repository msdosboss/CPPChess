#include "matchManager.hpp"

//std::mutex m; //refactored the globals out
//std::condition_variable cv;

int main(int argc, char **argv) {
    std::string fen = STARTFEN;
    for (int i = 0; i < argc; ++i) {
        std::string s = std::string(argv[i]);
        if (s == "-f") {
            fen = std::string(argv[i+1]);
            break;
        } 
    }

    std::thread userCLIThread;
    std::thread engineOneThread;
    std::thread engineTwoThread;
    std::atomic<bool> engineOneReady = false;
    std::atomic<bool> engineTwoReady = false;
    std::atomic<int> turnState;
    std::atomic<bool> gameOver = false;
    struct BoardState state;
    std::mutex threadSyncMutex;
    std::condition_variable mutexCondition;
    std::string UCIResponse; //locked behind the mutex condition
    bool responseReady = false; //locked behind the mutex condition
    
    fenToBoardState(fen, std::ref(state));
    turnState = state.sideToMove;

    userCLIThread = std::thread(
        CLIThread,
        std::ref(gameOver),
        std::ref(threadSyncMutex),
        std::ref(mutexCondition)        
    );
    engineOneThread = std::thread(
        engineThread,
        std::ref(engineOneReady),
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
        std::ref(engineTwoReady),
        std::ref(turnState),
        BLACK,
        std::ref(gameOver),
        std::ref(state),
        std::ref(UCIResponse),
        std::ref(responseReady),
        std::ref(threadSyncMutex),
        std::ref(mutexCondition)
    );

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
            //Skip "bestmove"
            ss >> token;
            //token equals move ie "e2e4"
            ss >> token;

            Move engineMove = strMoveToMove(token, state);

            UndoState undo;
            makeMove(state, engineMove, undo);
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

}

void engineThread(
    std::atomic<bool>& readyFlag,
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
    if ((sockDesc = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        std::cerr << "Failed to create socket in engineThread: color = " << color << std::endl;
    }
    struct sockaddr_in listenAddressOne = {
        .sin_family = AF_INET,
        .sin_port = htons(ENGINE_LISTEN_PORT[color]), //Color *must* be 0 or 1 (white, black)
        .sin_addr = {.s_addr = INADDR_ANY} //(man 7 ip)
    };
    const short int reuse = 1;
    setsockopt(sockDesc, SOL_SOCKET, SO_REUSEADDR, (void *) &reuse, sizeof(reuse));
    const int res = bind(sockDesc, (const struct sockaddr *)&listenAddressOne, sizeof(listenAddressOne));
    if (res == -1) {
        std::cerr << "Failed to bind socket. Errno=" << errno << std::endl;
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
        clientDesc = accept4(sockDesc, (struct sockaddr *) &clientConnInfo, &connSizeInfo, SOCK_NONBLOCK); //no longer blocks until connection
        if (clientDesc == -1) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                continue;
            } else {
                std::cerr << "Engine Thread (color=" << color << ") failed accept(), errno=" << errno << std::endl;
                return; //Probably should gracefully close down the threads and such at some point
                //For now, just returning early. TODO
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100)); //TODO - magic number
    } while (clientDesc == -1);
    //Could output the contents of clientConnInfo for logging / connection debug
    std::cerr << "Client successfully connected: "
        << ntohs(clientConnInfo.sin_addr.s_addr)
        << ":" << ntohs(clientConnInfo.sin_port) << std::endl;
    
    while (true) {
        std::unique_lock lk(m);
        cv.wait(lk, [color, &turnState, &gameOver]{ 
                return color == turnState || gameOver; 
        }); //my turn
        std::cerr << "matchManager engine thread woken up, color=" << color << std::endl;
        lk.unlock(); //This allows main thread to continue to act
        if(gameOver){
            char buf[] = "bye";
            send(clientDesc, buf, sizeof(buf), MSG_DONTWAIT);
            close(clientDesc); //this technically has a return value
                //to indicate failure, but it's dumb
            break;
        }

        lk.lock();
        std::string cmd = createPositionCmd(state);
        lk.unlock();
        char buf[PACKET_STR_SIZE];
        std::strncpy(buf, cmd.c_str(), PACKET_STR_SIZE);
        buf[PACKET_STR_SIZE - 1] = '\0';
        //send Position command
        send(clientDesc, buf, PACKET_STR_SIZE, MSG_MORE);
        cmd = "go";
        std::strncpy(buf, cmd.c_str(), PACKET_STR_SIZE); 
        buf[PACKET_STR_SIZE - 1] = '\0';
        //send Go command
        send(clientDesc, buf, PACKET_STR_SIZE, 0);
        //await engine response
        int bytesRead = recv(clientDesc, buf, PACKET_STR_SIZE - 1, 0);
        if(bytesRead <= 0){
            break;
        }
        //ensure std::string cast wont pickup garbage
        buf[bytesRead] = '\0';
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
}

void CLIThread(std::atomic<bool>& gameOver, std::mutex& m, std::condition_variable& cv){
    while(true){
        std::string userInput;
        std::cin >> userInput;
        if(userInput == "abort" || std::cin.eof()){
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
