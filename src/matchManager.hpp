#ifndef __MATCH_MANAGER_HPP__
#define __MATCH_MANAGER_HPP__

#include "physics.hpp"
#include <thread>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sstream>
#include <atomic>
#include <mutex>
#include <condition_variable>

void engineThread(std::atomic<bool>& readyFlag, const std::atomic<int>& turnState, int color, std::atomic<bool>& gameOver, std::mutex& m, std::condition_variable& cv);

#define LISTEN_PORT_WHITE 3001
#define LISTEN_PORT_BLACK 3002
const int ENGINE_LISTEN_PORT[2] = {LISTEN_PORT_WHITE, LISTEN_PORT_BLACK};

#endif // #ifndef __MATCH_MANAGER_HPP__
