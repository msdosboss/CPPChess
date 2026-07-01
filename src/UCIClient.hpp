#ifndef MAIN_HPP
#define MAIN_HPP

#include "physics.hpp"
#include "evaluate.hpp"
#include "openBook.hpp"
#include "gui.hpp"
#include "engineProcess.hpp"

#include <iostream>
#include <string>
#include <cstdint>
#include <sstream>
#include <SDL.h>
#include <SDL_image.h>
#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>

#define MAXMOVESPERGAME 1024

#define SIZE 100

void engineThread(
    BoardState& boardState,
    std::atomic<bool>& gameOver,
    int color,
    EngineProcess& engine,
    std::mutex& m,
    std::condition_variable& cv
);


void initTexture(SDL_Renderer *rend);

#endif
