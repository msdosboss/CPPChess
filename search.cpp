#include "search.hpp"

Move searchBestMove(BoardState& boardState, int depth, int& finalEval){
    MoveList legalMoves = generateLegalMoves(boardState);
    Move bestMove = legalMoves.moves[0];

    int alpha = -INFINITESCORE;
    int beta = INFINITESCORE;

    if(boardState.sideToMove == WHITE){
        int bestScore = -INFINITESCORE;
        for(int i = 0; i < legalMoves.count; i++){
            UndoState undo;
            Move currentMove = legalMoves.moves[i];
            makeMove(boardState, currentMove, undo);
            int currentMoveScore = minimax(boardState, depth - 1, alpha, beta);
            if(currentMoveScore > bestScore){
                bestMove = currentMove;
                bestScore = currentMoveScore;
            }
            alpha = std::max(alpha, bestScore);
            unmakeMove(boardState, currentMove, undo);
            if(beta <= alpha){
                break;
            }
        }
        finalEval = bestScore;
    }

    else{
        int bestScore = INFINITESCORE;
        for(int i = 0; i < legalMoves.count; i++){
            UndoState undo;
            Move currentMove = legalMoves.moves[i];
            makeMove(boardState, currentMove, undo);
            int currentMoveScore = minimax(boardState, depth - 1, alpha, beta);
            if(currentMoveScore < bestScore){
                bestMove = currentMove;
                bestScore = currentMoveScore;
            }
            beta = std::min(beta, bestScore);
            unmakeMove(boardState, currentMove, undo);
            if(beta <= alpha){
                break;
            }
        }
        finalEval = bestScore;
    }

    return bestMove;
}

int minimax(BoardState& boardState, int depth, int alpha, int beta){
    if(depth == 0){
        return evaluate(boardState);
    }
    MoveList legalMoves = generateLegalMoves(boardState);
    if(legalMoves.count == 0){
        if(boardState.sideToMove == WHITE){
            Bitboard kingbb = boardState.pieces[WHITE][KING];
            int kingIndex = __builtin_ctzll(kingbb);
            if(isSquareAttacked(boardState, kingIndex, BLACK)){
                //White checkmated
                //mate in 1 worse then mate in 5
                return -MATESCORE + depth;
            }
            else{
                //Draw
                return 0;
            }
        }
        else{
            Bitboard kingbb = boardState.pieces[BLACK][KING];
            int kingIndex = __builtin_ctzll(kingbb);
            if(isSquareAttacked(boardState, kingIndex, WHITE)){
                //Black checkmated
                //mate in 1 worse then mate in 5
                return MATESCORE - depth;
            }
            else{
                //Draw
                return 0;
            }
       }
    }
    if(boardState.sideToMove == WHITE){
        int maxScore = -INFINITESCORE;
        for(int i = 0; i < legalMoves.count; i++){
            Move currentMove = legalMoves.moves[i];
            UndoState undo;
            makeMove(boardState, currentMove, undo);
            int currentMoveScore = minimax(boardState, depth - 1, alpha, beta);
            maxScore = std::max(currentMoveScore, maxScore);
            alpha = std::max(alpha, currentMoveScore);
            unmakeMove(boardState, currentMove, undo);
            if(beta <= alpha){
                break;
            }
        }
        return maxScore;
    }
    else{
        int minScore = INFINITESCORE;
        for(int i = 0; i < legalMoves.count; i++){
            Move currentMove = legalMoves.moves[i];
            UndoState undo;
            makeMove(boardState, currentMove, undo);
            int currentMoveScore = minimax(boardState, depth - 1, alpha, beta);
            minScore = std::min(currentMoveScore, minScore);
            beta = std::min(beta, currentMoveScore);
            unmakeMove(boardState, currentMove, undo);
            if(beta <= alpha){
                break;
            }
        }

        return minScore;
    }
}
