#ifndef NETUTILS_HPP
#define NETUTILS_HPP

#include <queue>
#include <string>
#include <sstream>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>

#define PACKET_STR_SIZE 1024

class NetConnection{
    private:
        int sockfd;
        std::queue<std::string> recvQueue;
        std::string accumulatedResponse;
    public:
        NetConnection(int sock);
        int netRecv();
        int netSend(std::string msg);
        int netClose();
        std::string netDequeue();
        void setBlocking();
        void unsetBlocking();
};

void validateSend(int sendRetVal, int expectedByteCount);

#endif
