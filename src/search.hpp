#ifndef SEARCH_H
#define SEARCH_H

#include "physics.hpp"
#include "evaluate.hpp"
#include "transpositionTable.hpp"
#include "openBook.hpp"
#include <algorithm>
#include <chrono>
#include <atomic>

#define MATESCORE     20000
#define INFINITESCORE 200000

#define CAPTUREBIAS 1000

struct SearchInfo{
    std::atomic<bool> timesUp;
    std::chrono::steady_clock::time_point start;
    std::chrono::seconds duration;
    std::atomic<int> nodesSearched;
};

Move searchBestMove(BoardState& boardState, int depth, int& finalEval);
int minimax(BoardState& boardState, int depth, int alpha, int beta, SearchInfo& searchInfo);
int quiescenceSearch(BoardState& boardState, int depth, int alpha, int beta, SearchInfo& searchInfo);
Move searchBestMoveIt(BoardState boardState, int maxDepth, std::chrono::seconds duration, SearchInfo& searchInfo, int& finalEval);
void scoreMoves(BoardState& boardState, MoveList& moveList, Move bestMove);


#endif
