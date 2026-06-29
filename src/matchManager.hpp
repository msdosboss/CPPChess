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

struct GameState {
    std::atomic<bool> gameOver;
    std::atomic<bool> whiteReady;
    std::atomic<bool> blackReady;
    std::atomic<bool> timeUp;
    std::atomic<int> turnState;
    std::mutex threadSyncMutex;
    std::condition_variable mutexCondition;
    BoardState state;
};


void engineThread(
    struct GameState& gameState,
    int color,
    std::string& UCIResponse,
    bool& responseReady
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

void CLIThread(std::atomic<bool>& gameOver, std::atomic<bool>& timeUp, std::mutex& m, std::condition_variable& cv);

#define PACKET_STR_SIZE 128
#define LISTEN_PORT_WHITE 3001
#define LISTEN_PORT_BLACK 3002
const int ENGINE_LISTEN_PORT[2] = {LISTEN_PORT_WHITE, LISTEN_PORT_BLACK};

#endif // #ifndef __MATCH_MANAGER_HPP__
