#include "matchManager.hpp"

int main(int argc, char **argv) {
    std::string fen = STARTFEN;
    if (argc == 1) {
        std::cerr << "usage: <" << argv[0] << "> -p <port\n";
    }
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
    struct BoardState state;
    std::atomic<std::string> UCIResponse;
    
    fenToBoardState(fen, std::ref(state));
    turnState = state.sideToMove;

    engineOneThread = std::thread(engineThread, std::ref(engineOneReady), std::ref(turnState), WHITE, std::ref(UCIResponse));
    engineTwoThread = std::thread(engineThread, std::ref(engineTwoReady), std::ref(turnState), BLACK, std::ref(UCIResponse));

}

void engineThread(std::atomic<bool>& readyFlag, const std::atomic<int>& turnState, int color, std::atomic<std::string>& UCIResponse) {
    int sockDesc = -1;
    if ((sockDesc = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        std::cerr << "Failed to create socket in engineThread: color = " << color << std::endl;
    }
    bind(
            //TODO -- make sockets
    
    while (true) {
        if (color == turnState) { //my turn...
            //send move out
            //await engine response
            //send received move to main thread



        }

    }

}
