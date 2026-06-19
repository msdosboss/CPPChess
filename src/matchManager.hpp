#ifndef __MATCH_MANAGER_HPP__
#define __MATCH_MANAGER_HPP__

#include "physics.hpp"
#include <thread>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <atomic>
#include <mutex>
#include <condition_variable>

void engineThread(bool& readyFlag, const int& turnState, int color, bool& gameOver);

#endif // #ifndef __MATCH_MANAGER_HPP__
