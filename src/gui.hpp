#ifndef GUI_H
#define GUI_H

#define SCREEN_WIDTH 800
#define SQUARE_WIDTH (SCREEN_WIDTH / 8)
#define SCREEN_HEIGHT 800
#define SQUARE_HEIGHT (SCREEN_HEIGHT / 8)
#define TARGET_FPS 60
#define FRAME_DELAY 1000 / TARGET_FPS

#include "physics.hpp"

#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include <atomic>
#include <mutex>
#include <condition_variable>

void renderBoard(
    BoardState& boardState,
    std::atomic<bool>& gameOver,
    uint32_t squareDarkColor,
    uint32_t squareLightColor,
    std::string textureDir,
    int playerSide,
    std::string& moveMadeStr,
    std::mutex& m,
    std::condition_variable& cv);
void initTexture(SDL_Renderer *rend, std::string textureDir);
void freeTextures();

#endif
