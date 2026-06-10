#include "search.hpp"

Move searchBestMoveIt(BoardState& boardState, int maxDepth, int& finalEval, std::chrono::seconds duration){
    Move topMove;
    SearchInfo searchInfo;
    searchInfo.timesUp = false;
    searchInfo.start = std::chrono::steady_clock::now();
    searchInfo.duration = duration;
    searchInfo.nodesSearched = 0;

    Move openMove;
    if(getBookMove(boardState.zobristHash, openMove)){
        return openMove;
    }

    for(int currentDepth = 1; currentDepth <= maxDepth; currentDepth++){
        int score = minimax(boardState, currentDepth, -INFINITESCORE, INFINITESCORE, searchInfo);

        if(searchInfo.timesUp == true){
            break;
        }
        
        TTEntry rootEntry;
        if(TT.probe(boardState.zobristHash, rootEntry)){
            topMove = rootEntry.bestMove;
            finalEval = score;
        }
    }
    return topMove;
}

void scoreMoves(BoardState& boardState, MoveList& moveList, Move bestMove){
    for(int i = 0; i < moveList.count; i++){
        if(moveList.moves[i].raw == bestMove.raw){
            moveList.moveScores[i] = INFINITESCORE;
            continue;
        }
        int sourcePieceType = boardState.pieceArray[moveList.moves[i].source];
        int destPieceType = boardState.pieceArray[moveList.moves[i].dest];
        /*for(int color = WHITE; color <= BLACK; color++){
            for(int pieceType = PAWN; pieceType <= KING; pieceType++){
                if(isOccupied(boardState.pieces[color][pieceType], moveList.moves[i].source)){
                    sourcePieceType = pieceType;
                }
                if(isOccupied(boardState.pieces[color][pieceType], moveList.moves[i].dest)){
                    destPieceType = pieceType;
                }
            }
        }*/
        if(destPieceType == -1){
            moveList.moveScores[i] = 0;
        }
        else{
            moveList.moveScores[i] = 100 + (10 * destPieceType) - sourcePieceType;
        }
    }
}

Move searchBestMove(BoardState& boardState, int depth, int& finalEval){
    MoveList legalMoves = generateLegalMoves(boardState);
    Move bestMove = legalMoves.moves[0];
    SearchInfo searchInfo;
    searchInfo.timesUp = false;

    int alpha = -INFINITESCORE;
    int beta = INFINITESCORE;

    if(boardState.sideToMove == WHITE){
        int bestScore = -INFINITESCORE;
        for(int i = 0; i < legalMoves.count; i++){
            UndoState undo;
            Move currentMove = legalMoves.moves[i];
            makeMove(boardState, currentMove, undo);
            int currentMoveScore = minimax(boardState, depth - 1, alpha, beta, searchInfo);
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
            int currentMoveScore = minimax(boardState, depth - 1, alpha, beta, searchInfo);
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

const uint8_t FLAG_EXACT = 0;
const uint8_t FLAG_ALPHA = 1; // Upperbound
const uint8_t FLAG_BETA = 2;  // Lowerbound

int minimax(BoardState& boardState, int depth, int alpha, int beta, SearchInfo& searchInfo){
    searchInfo.nodesSearched++;
    if((searchInfo.nodesSearched & 2047) == 0){
        std::chrono::steady_clock::time_point current = std::chrono::steady_clock::now();
        if(current - searchInfo.start > searchInfo.duration){
            searchInfo.timesUp = true;
            return 0;
        }
    }
    TTEntry ttEntry;
    Move possibleBestMove = {0};
    if (TT.probe(boardState.zobristHash, ttEntry)) {
        if(ttEntry.depth >= depth){
            if (ttEntry.flag == FLAG_EXACT) {
                return ttEntry.score;
            }
            if (ttEntry.flag == FLAG_ALPHA && ttEntry.score <= alpha) {
                return ttEntry.score;
            }
            if (ttEntry.flag == FLAG_BETA && ttEntry.score >= beta) {
                return ttEntry.score;
            }
        }
        possibleBestMove = ttEntry.bestMove;
    }
    if(depth == 0){
        //return evaluate(boardState);
        return quiescenceSearch(boardState, 5, alpha, beta, searchInfo);
    }
    MoveList legalMoves = generateLegalMoves(boardState);
    scoreMoves(boardState, legalMoves, possibleBestMove);
    if(legalMoves.count == 0){
        if(boardState.sideToMove == WHITE){
            Bitboard kingbb = boardState.pieces[WHITE][KING];
            int kingIndex = __builtin_ctzll(kingbb);
            if(isSquareAttacked(boardState, kingIndex, BLACK)){
                //White checkmated
                //mate in 1 worse then mate in 5
                return -MATESCORE - depth;
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
                return MATESCORE + depth;
            }
            else{
                //Draw
                return 0;
            }
       }
    }
    //moveSwap(legalMoves, possibleBestMove);
    Move bestMove = legalMoves.moves[0];
    if(boardState.sideToMove == WHITE){
        int maxScore = -INFINITESCORE;
        int originalAlpha = alpha;
        for(int i = 0; i < legalMoves.count; i++){
            int bestMoveIndex = i;
            for(int j = i + 1; j < legalMoves.count; j++){
                if(legalMoves.moveScores[j] > legalMoves.moveScores[bestMoveIndex]){
                    bestMoveIndex = j;
                }
            }
            std::swap(legalMoves.moves[i], legalMoves.moves[bestMoveIndex]);
            std::swap(legalMoves.moveScores[i], legalMoves.moveScores[bestMoveIndex]);
            Move currentMove = legalMoves.moves[i];
            UndoState undo;
            makeMove(boardState, currentMove, undo);
            int currentMoveScore = minimax(boardState, depth - 1, alpha, beta, searchInfo);
            if(searchInfo.timesUp == true){
                return 0;
            }
            if(currentMoveScore > maxScore){
                maxScore = currentMoveScore;
                bestMove = legalMoves.moves[i];
            }
            alpha = std::max(alpha, currentMoveScore);
            unmakeMove(boardState, currentMove, undo);
            if(beta <= alpha){
                break;
            }
        }
        uint8_t flag;
        if (maxScore <= originalAlpha) {
            // We never improved our starting situation. This is a ceiling.
            flag = FLAG_ALPHA;
        }
        else if (maxScore >= beta) {
            // We broke the beta limit. This is a floor.
            flag = FLAG_BETA;
        }
        else {
            // The score landed perfectly between originalAlpha and beta.
            flag = FLAG_EXACT;
        }
        TT.store(boardState.zobristHash, depth, maxScore, flag, bestMove);
        return maxScore;
    }
    else{
        int minScore = INFINITESCORE;
        int originalBeta = beta;
        for(int i = 0; i < legalMoves.count; i++){
            int bestMoveIndex = i;
            for(int j = i + 1; j < legalMoves.count; j++){
                if(legalMoves.moveScores[j] > legalMoves.moveScores[bestMoveIndex]){
                    bestMoveIndex = j;
                }
            }
            std::swap(legalMoves.moves[i], legalMoves.moves[bestMoveIndex]);
            std::swap(legalMoves.moveScores[i], legalMoves.moveScores[bestMoveIndex]);
            Move currentMove = legalMoves.moves[i];
            UndoState undo;
            makeMove(boardState, currentMove, undo);
            int currentMoveScore = minimax(boardState, depth - 1, alpha, beta, searchInfo);
            if(searchInfo.timesUp == true){
                return 0;
            }
            if(currentMoveScore < minScore){
                minScore = currentMoveScore;
                bestMove = legalMoves.moves[i];
            }
            beta = std::min(beta, currentMoveScore);
            unmakeMove(boardState, currentMove, undo);
            if(beta <= alpha){
                break;
            }
        }
        uint8_t flag;
        if (minScore <= alpha) {
            // We never improved our starting situation. This is a ceiling.
            flag = FLAG_ALPHA;
        }
        else if (minScore >= originalBeta) {
            // We broke the beta limit. This is a floor.
            flag = FLAG_BETA;
        }
        else {
            // The score landed perfectly between originalAlpha and beta.
            flag = FLAG_EXACT;
        }
        TT.store(boardState.zobristHash, depth, minScore, flag, bestMove);
        return minScore;
    }

}

int quiescenceSearch(BoardState& boardState, int depth, int alpha, int beta, SearchInfo& searchInfo){
    searchInfo.nodesSearched++;
    if((searchInfo.nodesSearched & 2047) == 0){
        std::chrono::steady_clock::time_point current = std::chrono::steady_clock::now();
        if(current - searchInfo.start > searchInfo.duration){
            searchInfo.timesUp = true;
            return 0;
        }
    }
    if(depth == 0){
        return evaluate(boardState);
    }
    int standardPat = evaluate(boardState);
    if(boardState.sideToMove == WHITE){
        if(standardPat >= beta){
            return standardPat;
        }
        alpha = std::max(alpha, standardPat);
    }
    else{
        if(standardPat <= alpha){
            return standardPat;
        }
        beta = std::min(beta, standardPat);
    }
    MoveList legalMoves = generateLegalMoves(boardState);
    if(boardState.sideToMove == WHITE){
        int maxScore = standardPat;
        for(int i = 0; i < legalMoves.count; i++){
            Move currentMove = legalMoves.moves[i];
            if(currentMove.flags ==  QUIETMOVE || 
               currentMove.flags ==  DOUBLEMOVE||
               currentMove.flags ==  KINGCASTLE||
               currentMove.flags == QUEENCASTLE){
                continue; 
            }
            UndoState undo;
            makeMove(boardState, currentMove, undo);
            int currentMoveScore = quiescenceSearch(boardState, depth - 1, alpha, beta, searchInfo);
            if(searchInfo.timesUp == true){
                return 0;
            }
            alpha = std::max(alpha, currentMoveScore);
            maxScore = std::max(currentMoveScore, maxScore);
            unmakeMove(boardState, currentMove, undo);
            if(beta <= alpha){
                break;
            }
        }
        return maxScore;
    }
    else{
        int minScore = standardPat;
        for(int i = 0; i < legalMoves.count; i++){
            Move currentMove= legalMoves.moves[i];
            if(currentMove.flags ==  QUIETMOVE || 
               currentMove.flags ==  DOUBLEMOVE||
               currentMove.flags ==  KINGCASTLE||
               currentMove.flags == QUEENCASTLE){
                continue; 
            }
            UndoState undo;
            makeMove(boardState, currentMove, undo);
            int currentMoveScore = quiescenceSearch(boardState, depth - 1, alpha, beta, searchInfo);
            if(searchInfo.timesUp == true){
                return 0;
            }
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
