#ifndef SEARCH_H
#define SEARCH_H

#include "physics.hpp"
#include "evaluate.hpp"
#include <algorithm>

#define MATESCORE     20000
#define INFINITESCORE 200000

Move searchBestMove(BoardState& boardState, int depth, int& finalEval);
int minimax(BoardState& boardState, int depth, int alpha, int beta);

#endif
