#include "netClient.hpp"

int main(int argc, char **argv)
{
    uint32_t port = 0;
    std::string pathToEngine = "./engine";
    if (argc == 1) {
        std::cerr << "usage: <" << argv[0] << "> -p <port\n";
    }
    for (int i = 0; i < argc; ++i) {
        std::string s = std::string(argv[i]);
        if (s == "-p") {
            port = std::atoi(argv[i+1]);
            break;
        } 
        else if (s == "-a") {
            pathToEngine = std::string(argv[i+1]);
        }

    }

    if (port == 0) {
        std::cerr << "Could not set port" << std::endl;
        return -1;
    }

    std::thread serverThread;
    EngineProcess engine(pathToEngine);

    

    return 0;
}


