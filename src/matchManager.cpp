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

    engineOneThread = std::thread(
        engineThread,
        std::ref(engineOneReady),
        std::ref(turnState),
        WHITE,
        std::ref(gameOver),
        threadSyncMutex,
        mutexCondition
    );
    engineTwoThread = std::thread(
        engineThread,
        std::ref(engineTwoReady),
        std::ref(turnState),
        BLACK,
        std::ref(gameOver),
        threadSyncMutex,
        mutexCondition
    );

    while(true){
        std::unique_lock lk(m);
        cv.wait(lk, []{ return responseReady;});
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
            responseReady = false;
            turnState = state.sideToMove;
        }
    }

    if(engineOneThread.joinable()){
        engineOneThread.join();
    }
    if(engineTwoThread.joinable()){
        engineTwoThread.join();
    }

}

void engineThread(
    std::atomic<bool>& readyFlag,
    const std::atomic<int>& turnState,
    int color,
    std::atomic<bool>& gameOver,
    std::mutex m,
    std::condition_variable cv
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
    bind(sockDesc, listenAddressOne, sizeof(listenAddressOne));

    const int connectionBacklogLimit = 1;
    listen(sockDesc, connectionBacklogLimit);
    struct sockaddr_in clientConnInfo; //filled in with call to accept()
    socklen_t connSizeInfo = sizeof(clientConnInfo); //will be overwritten by accept()
    accept(sockDesc, &clientConnInfo, &connSizeInfo); //block until connection
    //Could output the contents of clientConnInfo for logging / connection debug
    
    while (true) {
        std::unique_lock lk(m);
        cv.wait(lk, [color, &turnState, &gameOver]{ 
                return color == turnState || gameOver; 
        }); //my turn
        if(gameOver){
            break;
        }

        //send move out
        //await engine response
        //send received move to main thread
        responseReady = true;
        cv.notify_all();
        cv.wait(lk, []{ return !responseReady;});

        lk.unlock();
        cv.notify_all();

        //break;

    }

}
