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


    Packet recvPacket;
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
        

    std::mutex threadMutex;
    
    std::thread serverThread;
    try {
        serverThread = std::thread(
            serverListener,
            sockDesc,
            std::ref(recvFlag),
            std::ref(recvPacket),
            std::ref(threadMutex)
        );
    } catch (std::system_error& e) {
        std::cerr << "Blew up at server thread creation" << std::endl;
        return -1;
    }
    EngineProcess engine(pathToEngine);
    Packet sendPacket = {0};

    while (true) {
        std::unique_lock lk(threadMutex);
        if (engine.hasData()) {
            std::string engineResponse = engine.receiveCommand();
            strncpy(sendPacket.str, engineResponse.c_str(), PACKET_STR_SIZE - 1);
            sendPacket.str[PACKET_STR_SIZE - 1] = '\0';
            send(sockDesc, (void *) sendPacket.str, PACKET_STR_SIZE, 0);
        }
        if (std::string(recvPacket.str) == "bye") {
            break;
        }
        if (recvFlag) {
            std::cout << "Sending: " << std::string(recvPacket.str) << "to engine" << std::endl;
            engine.sendCommand(std::string(recvPacket.str));
            recvFlag = false;
        }
        lk.unlock();
        //Took this from seconds to milliseconds because 1 second is to long for engine
        std::this_thread::sleep_for(std::chrono::milliseconds(1)); //small hack: avoid cpu busy wait
    }

    if (serverThread.joinable()) {
        serverThread.join();
    }

    return 0;
}

void serverListener(
    const int socketFD,
    std::atomic<bool>& recvFlag,
    struct Packet& recvPacket,
    std::mutex& m
) {
    try {
        while (true) {
            char buf[PACKET_STR_SIZE];
            int bytesRead = recv(socketFD, (void *) buf, PACKET_STR_SIZE - 1, 0);
            if (bytesRead <= 0) {
                //Server disconnected or error occurred
                break;
            }
            buf[bytesRead] = '\0';
            std::cout << "recv loaded " << std::string(buf) << std::endl;
            std::cout << "bytesRead = " << bytesRead << std::endl;
            recvFlag = true;
            std::unique_lock lk(m);
            //lk.lock();
            std::strncpy(recvPacket.str, buf, bytesRead + 1);

            if (std::string(recvPacket.str) == "bye") {
                lk.unlock();
                break;
            }
            lk.unlock();
        }
    } catch (std::system_error& e) {
        std::cerr << "Blew up in serverListener" << std::endl;
        std::exit(-1);
    }
}
