#ifndef __NETCLIENT_HPP__
#define __NETCLIENT_HPP__

#include "UCIClient.hpp"
#include "physics.hpp"

#include <sys/socket.h>
#include <arpa/inet.h>
#include <thread>
#include "engineProcess.hpp"
#include <string.h>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <sstream>

#define PACKET_STR_SIZE 128
struct Packet {
    char str[PACKET_STR_SIZE];
};
void serverListener(const int socketFD, std::atomic<bool>& recvFlag, struct Packet& recvPacket, std::mutex& m);

void humanServerListner(
    const int socketFD,
    BoardState& boardState,
    std::atomic<bool>& guiNeedsToMove,
    std::atomic<bool>& gameOver,
    std::mutex& m,
    std::condition_variable& cv 
);
void humanSender(
    const int socketFD,
    std::string& moveMadeStr,
    std::atomic<bool>& guiNeedsToMove,
    std::atomic<bool>& gameOver,
    std::mutex& m,
    std::condition_variable& cv
);
void applyPositionCommand(const std::string& cmd, BoardState& boardState);


#endif //__NETCLIENT_HPP__
