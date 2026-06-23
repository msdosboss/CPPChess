#ifndef MAIN_HPP
#define MAIN_HPP

#include "physics.hpp"
#include "evaluate.hpp"
#include "openBook.hpp"

#include <iostream>
#include <string>
#include <cstdint>
#include <sstream>
#include <SDL.h>
#include <SDL_image.h>

#define MAXMOVESPERGAME 1024

#define SIZE 100

class IChessPlayer {
public:
    virtual ~IChessPlayer() = default;
    virtual void sendCommand(const std::string& cmd) = 0;
    virtual std::string receiveCommand() = 0;
    virtual bool hasData() = 0; 
};

void initTexture(SDL_Renderer *rend);

#endif
