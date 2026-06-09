#ifndef SEARCH_H
#define SEARCH_H

#include "physics.hpp"
#include "evaluate.hpp"
#include "transpositionTable.hpp"
#include "openBook.hpp"
#include <algorithm>
#include <chrono>

#define MATESCORE     20000
#define INFINITESCORE 200000

#define CAPTUREBIAS 1000

struct SearchInfo{
    bool timesUp;
    std::chrono::steady_clock::time_point start;
    std::chrono::seconds duration;
    int nodesSearched;
};

Move searchBestMove(BoardState& boardState, int depth, int& finalEval);
int minimax(BoardState& boardState, int depth, int alpha, int beta, SearchInfo& searchInfo);
int quiescenceSearch(BoardState& boardState, int depth, int alpha, int beta, SearchInfo& searchInfo);
Move searchBestMoveIt(BoardState& boardState, int maxDepth, int& finalEval, std::chrono::seconds duration);
void scoreMoves(BoardState& boardState, MoveList& moveList, Move bestMove);


#endif
