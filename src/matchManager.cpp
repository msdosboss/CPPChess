#include "matchManager.hpp"

//std::mutex m; //refactored the globals out
//std::condition_variable cv;
std::string UCIResponse;
bool responseReady = false;

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
        std::ref(threadSyncMutex),
        std::ref(mutexCondition)
    );

    while(true){
        std::unique_lock lk(threadSyncMutex);
        mutexCondition.wait(lk, [&gameOver]{ return gameOver || responseReady;});
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
    bind(sockDesc, (const struct sockaddr *)&listenAddressOne, sizeof(listenAddressOne));

    const int connectionBacklogLimit = 1;
    listen(sockDesc, connectionBacklogLimit);
    struct sockaddr_in clientConnInfo; //filled in with call to accept()
    socklen_t connSizeInfo = sizeof(clientConnInfo); //will be overwritten by accept()
    //At some point we probably want to make this a non-blocking loop
    //because we want to be able to abort without waiting for engines to connect
    accept(sockDesc, (struct sockaddr *)&clientConnInfo, &connSizeInfo); //block until connection
    //Could output the contents of clientConnInfo for logging / connection debug
    
    while (true) {
        std::unique_lock lk(m);
        cv.wait(lk, [color, &turnState, &gameOver]{ 
                return color == turnState || gameOver; 
        }); //my turn
        lk.unlock(); //This allows main thread to continue to act
        if(gameOver){
            break;
        }

        lk.lock();
        std::string cmd = createPositionCmd(state);
        lk.unlock();
        char buf[PACKET_STR_SIZE];
        std::strncpy(buf, cmd.c_str(), PACKET_STR_SIZE);
        buf[PACKET_STR_SIZE - 1] = '\0';
        //send Position command
        send(sockDesc, buf, PACKET_STR_SIZE, 0);
        cmd = "go";
        std::strncpy(buf, cmd.c_str(), PACKET_STR_SIZE); 
        buf[PACKET_STR_SIZE - 1] = '\0';
        //send Go command
        send(sockDesc, buf, PACKET_STR_SIZE, 0);
        //await engine response
        int bytesRead = recv(sockDesc, buf, PACKET_STR_SIZE - 1, 0);
        if(bytesRead <= 0){
            break;
        }
        //ensure std::string cast wont pickup garbage
        buf[bytesRead] = '\0';
        lk.lock(); //This ensure that it is safe to write to the global UCIResponse
        //send received move to main thread
        UCIResponse = std::string(buf);
        responseReady = true;
        lk.unlock();
        cv.notify_all();
    }
}

void CLIThread(std::atomic<bool>& gameOver, std::mutex& m, std::condition_variable& cv){
    while(true){
        std::string userInput;
        std::cin >> userInput;
        if(userInput == "abort"){
            std::unique_lock lk(m);
            gameOver = true;
            lk.unlock();
            cv.notify_all();
            break;
        }
        else{
            std::cout << "Unknown Coammand: " << userInput << std::endl;
        }
    }
}
