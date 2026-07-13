#include "engineClient.hpp"

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

	std::atomic<bool> shouldClose = false;

	std::thread cliThread(miniCLI, std::ref(shouldClose));

	do {
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
		const int reuse = 1;
		if (setsockopt(sockDesc, SOL_SOCKET, SO_REUSEADDR, (void *) &reuse, sizeof(reuse)) == -1) {
			std::cerr << "setsockopt failed with errno=" << errno << std::endl;
			close(sockDesc);
			return -1;
		}
		/* Note -- Retain for socket refactor
		if (connect(sockDesc, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) == -1) {
			std::cerr << "Failed to connect with socket." << std::endl;
			return -1;
		}*/

		//threadMutex locks access to recvPacket and recvFlag
		std::mutex threadMutex;
		Packet recvPacket;
		std::atomic<bool> recvFlag = false;


		while (true) { //TODO -- remove in socket refactor
			if (connect(sockDesc, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) == -1) {
				if (errno == ECONNREFUSED) continue;
				std::cerr << "Failed to connect with socket. errno=" << errno << std::endl;
				cliThread.detach();
				return -1;
			} else {
				break;
			}
		}
        NetConnection clientCon(sockDesc);
		std::thread serverThread(
			serverListener,
			clientCon,
			std::ref(recvFlag),
			std::ref(recvPacket),
			std::ref(threadMutex)
		);
		EngineProcess engine(pathToEngine);
		Packet sendPacket = {0};

		while (true) {
			std::unique_lock lk(threadMutex);
			if (engine.hasData()) {
				std::string engineResponse = engine.receiveCommand();
				std::cerr << "Sending engine response of {{ " << engineResponse << " }}\n";
				engineResponse += "\n";
                clientCon.netSend(engineResponse);
			}
			if (std::string(recvPacket.str) == "bye") {
				std::cerr << "exiting: received shutdown command" << std::endl;
                clientCon.netClose();
				break;
			}
			if (recvFlag) {
				std::cout << "Sending: {" << std::string(recvPacket.str) << "} to engine" << std::endl;
				engine.sendCommand(std::string(recvPacket.str));
				recvFlag = false;
			}
			lk.unlock();
			//Took this from seconds to milliseconds because 1 second is to long for engine
			std::this_thread::sleep_for(std::chrono::milliseconds(1)); //small hack: avoid cpu busy wait
		}

		close(sockDesc);
		serverThread.join();
	} while (!shouldClose);
	cliThread.join();

    return 0;
}

void miniCLI(std::atomic<bool>& shouldClose) {
	while (true) {
		std::string str;
		std::cin >> str;
		if(str == "abort"){
			shouldClose = true;
			return;
		}
		else{
			std::cerr << str << " is an unknown command (type abort to quit)" << std::endl;
		}
	}
}
