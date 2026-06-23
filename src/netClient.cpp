#include "netClient.hpp"


int main(int argc, char **argv)
{
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


    std::atomic<Packet> recvPacket;
    std::atomic<bool> recvFlag = false;

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
        

    
    std::thread serverThread(serverListener, sockDesc, std::ref(recvFlag), std::ref(recvPacket));
    EngineProcess engine(pathToEngine);

    Packet sendPacket = {0};

    while (true) {
        struct Packet local = recvPacket.load();
        if (engine.hasData()) {
            strncpy(sendPacket.str, engine.receiveCommand().c_str(), PACKET_STR_SIZE - 1);
            sendPacket.str[127] = '\0';
            send(sockDesc, (void *) sendPacket.str, PACKET_STR_SIZE, 0);
        }
        else if (recvFlag) {
            engine.sendCommand(std::string(local.str));
            recvFlag = false;
        }
        if (std::string(local.str) == "bye") {
            break;
        }
        //Took this from seconds to milliseconds because 1 second is to long for engine
        std::this_thread::sleep_for(std::chrono::milliseconds(1)); //small hack: avoid cpu busy wait
    }

    if (serverThread.joinable()) {
        serverThread.join();
    }

    return 0;
}

// Uses global state: sendPacket
void serverListener(const int socketFD, std::atomic<bool>& recvFlag, std::atomic<struct Packet>& recvPacket) {
    while (true) {
        struct Packet localPacket = recvPacket.load();
        struct Packet localDiff = localPacket;
        char buf[PACKET_STR_SIZE];
        int bytesRead = recv(socketFD, (void *) buf, PACKET_STR_SIZE, 0);
        if (bytesRead <= 0) {
            //Server disconnected or error occurred
            break;
        }
        recvFlag = true;

        do {
            localDiff = localPacket;
            memcpy(localDiff.str, buf, PACKET_STR_SIZE);
        } while (recvPacket.compare_exchange_weak(localPacket, localDiff));
        if (std::string(localPacket.str) == "bye") {
            break;
        }
    }
}
