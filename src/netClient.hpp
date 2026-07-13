#ifndef __NETCLIENT_HPP__
#define __NETCLIENT_HPP__

#include "UCIClient.hpp"
#include "physics.hpp"
#include "netUtils.hpp"

#include <sys/socket.h>
#include <arpa/inet.h>
#include <thread>
#include "engineProcess.hpp"
#include <string.h>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <sstream>

void serverListener(
	NetConnection clientCon,
	std::atomic<bool>& recvFlag,
	std::string& recvStr,
	std::mutex& m
);

void humanServerListener(
	NetConnection clientCon,
    BoardState& boardState,
    std::atomic<bool>& guiNeedsToMove,
    std::atomic<bool>& gameOver,
    std::mutex& m,
    std::condition_variable& cv 
);
void humanSender(
	NetConnection clientCon,
    std::string& moveMadeStr,
    std::atomic<bool>& guiNeedsToMove,
    std::atomic<bool>& gameOver,
    std::mutex& m,
    std::condition_variable& cv
);
void applyPositionCommand(const std::string& cmd, BoardState& boardState);


#endif //__NETCLIENT_HPP__
