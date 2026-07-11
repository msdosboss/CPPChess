#ifndef NETUTILS_HPP
#define NETUTILS_HPP

#include <queue>
#include <string>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>

class NetConnection{
    private:
        int sockfd;
        std::queue<std::string> recvQueue;
    public:
        int netRecv();
        int netSend(std::string msg);
        int netClose();
        std::string dequeue();
};

void validateSend(int sendRetVal, int expectedByteCount);

#endif
