#ifndef __MATCH_MANAGER_HPP__
#define __MATCH_MANAGER_HPP__

#include "physics.hpp"
#include "gui.hpp"
#include <thread>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sstream>
#include <string>
#include <cstring>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <unistd.h>
#include <fcntl.h>
#include <cassert>

void engineThread(
    const std::atomic<int>& turnState,
    int color,
    std::atomic<bool>& gameOver,
    BoardState& state,
    std::string& UCIResponse,
    bool& responseReady,
    std::mutex& m,
    std::condition_variable& cv
);
void matchManagerThread(
    std::atomic<bool>& gameOver,
    std::atomic<int>& turnState,
    bool& responseReady,
    BoardState& state,
    std::string& UCIResponse,
    std::mutex& threadSyncMutex,
    std::condition_variable& mutexCondition
);

void CLIThread(std::atomic<bool>& gameOver, std::mutex& m, std::condition_variable& cv);

#define PACKET_STR_SIZE 128
#define LISTEN_PORT_WHITE 3001
#define LISTEN_PORT_BLACK 3002
const int ENGINE_LISTEN_PORT[2] = {LISTEN_PORT_WHITE, LISTEN_PORT_BLACK};

#endif // #ifndef __MATCH_MANAGER_HPP__
