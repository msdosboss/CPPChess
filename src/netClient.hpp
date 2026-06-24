#ifndef __NETCLIENT_HPP__
#define __NETCLIENT_HPP__

#include "UCIClient.hpp"

#include <sys/socket.h>
#include <arpa/inet.h>
#include <thread>
#include "engineProcess.hpp"
#include <string.h>
#include <atomic>
#include <mutex>

#define PACKET_STR_SIZE 128
struct Packet {
    char str[PACKET_STR_SIZE];
};
void serverListener(const int socketFD, std::atomic<bool>& recvFlag, struct Packet& recvPacket, std::mutex& m);

#endif //__NETCLIENT_HPP__
