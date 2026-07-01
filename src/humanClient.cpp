#include "humanClient.hpp"

int main(int argc, char **argv){
    char *port = NULL;
    char *ipAddress = NULL;
    std::string pathToEngine = "./engine";
    if (argc == 1) {
        std::cerr << "usage: <" << argv[0] << " -p <port>\n";
    }
    for (int i = 0; i < argc; ++i) {
        std::string s = std::string(argv[i]);
        if (s == "-p") {
            port = argv[i+1];
        }
        else if (s == "-a") {
            pathToEngine = std::string(argv[i+1]);
        }
        else if (s == "-i") {
            ipAddress = argv[i+1];
        }

    }
    if (port == NULL) {
        std::cerr << "Could not set port" << std::endl;
        return -1;
    }
    int playerColor;
    if(std::string(port) == "3001"){
        playerColor = WHITE;
    }
    else{
        playerColor = BLACK;
    }

    int sockDesc = socket(AF_INET, SOCK_STREAM, 0);
    if (sockDesc == -1) {
        std::cerr << "Failed to create socket." << std::endl;
        return -1;
    }

    sockaddr_in serverAddress = {
        .sin_family = AF_INET,
        .sin_port = htons(std::atoi(port))
    };
    char home[] = "127.0.0.1";
    if (ipAddress == NULL) ipAddress = home;
    if (inet_pton(AF_INET, ipAddress, &serverAddress.sin_addr) != 1) { //returns 1 on success
        std::cerr << "Failed to convert provided IP address" << std::endl;
        return -1;
    }
    if (connect(sockDesc, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) == -1) {
        std::cerr << "Failed to connect with socket." << std::endl;
        return -1;
    }
    generateKingAttacks();
    generateKnightAttacks();

    std::string msg = "id name humanClient";
    //Match Manager expects a name for the player
    send(sockDesc, msg.c_str(), msg.length(), 0);

    std::mutex threadMutex;
    std::condition_variable mutexCondition;
    BoardState boardState;
    fenToBoardState(STARTFEN, boardState);

    std::string lastMoveStr = "";
    std::atomic<bool> gameOver = false;
    std::atomic<bool> guiNeedsToMove = false;

    std::thread serverThread(
        humanServerListener, // You will write this variant
        sockDesc,
        std::ref(boardState),
        std::ref(guiNeedsToMove),
        std::ref(gameOver),
        std::ref(threadMutex),
        std::ref(mutexCondition)
    );

    std::thread senderThread(
        humanSender,
        sockDesc,
        std::ref(lastMoveStr),
        std::ref(guiNeedsToMove),
        std::ref(gameOver),
        std::ref(threadMutex),
        std::ref(mutexCondition)       
    );

    //Start the GUI on the Main Thread
    uint32_t lightColor = 0xffffffff;
    uint32_t darkColor = 0xff4a9627;
    renderBoard(
        std::ref(boardState),
        std::ref(gameOver),
        darkColor,
        lightColor,
        "img",
        playerColor,
        std::ref(lastMoveStr),
        std::ref(threadMutex),
        std::ref(mutexCondition)
    );

    gameOver = true;
    mutexCondition.notify_all();

    if(serverThread.joinable()){ 
        serverThread.join();
    }
    if(senderThread.joinable()){ 
        senderThread.join();
    }

    return 0;
}
