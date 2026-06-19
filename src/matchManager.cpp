#include "matchManager.hpp"

std::mutex m;
std::condition_variable cv;
std::string UCIResponse;

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
    
    fenToBoardState(fen, std::ref(state));
    turnState = state.sideToMove;

    engineOneThread = std::thread(engineThread, std::ref(engineOneReady), std::ref(turnState), WHITE, std::ref(gameOver));
    engineTwoThread = std::thread(engineThread, std::ref(engineTwoReady), std::ref(turnState), BLACK, std::ref(gameOver));

    if(engineOneThread.joinable()){
        engineOneThread.join();
    }
    if(engineTwoThread.joinable()){
        engineTwoThread.join();
    }

}

void engineThread(std::atomic<bool>& readyFlag, const std::atomic<int>& turnState, int color, std::atomic<bool>& gameOver) {
    int sockDesc = -1;
    if ((sockDesc = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        std::cerr << "Failed to create socket in engineThread: color = " << color << std::endl;
    }
    //bind(
            //TODO -- make sockets
    
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


        lk.unlock();
        cv.notify_all();

        //break;

    }

}
