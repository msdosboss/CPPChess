#ifndef __MATCH_MANAGER_HPP__
#define __MATCH_MANAGER_HPP__

#include "physics.hpp"
#include <thread>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <atomic>
#include <mutex>
#include <condition_variable>

void engineThread(std::atomic<bool>& readyFlag, const std::atomic<int>& turnState, int color);

#endif // #ifndef __MATCH_MANAGER_HPP__
