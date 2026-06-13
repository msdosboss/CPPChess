#ifndef ENGINE_H
#define ENGINE_H

#include <thread>
#include <atomic>
#include <iostream>
#include <string>
#include <sstream>
#include "physics.hpp"
#include "search.hpp"
#include "openBook.hpp"
#include "transpositionTable.hpp"

void runSearchWrapper(BoardState boardState, int maxDepth, std::chrono::seconds duration, SearchInfo& searchInfo);

#endif
