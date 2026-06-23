#ifndef ENGINE_HPP
#define ENGINE_HPP

#include <cstdint>
#include <string>
#include <iostream>

typedef uint64_t Bitboard;

#define MAXMOVES 218
#define STARTFEN "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"

#define WHITE 0
#define BLACK 1

#define PAWN 0
#define BISHOP 1
#define KNIGHT 2
#define ROOK 3
#define QUEEN 4
#define KING 5

union CastlingRights{
    uint8_t raw;
    struct {
        uint8_t queenSideCastleBlack: 1;
        uint8_t kingSideCastleBlack: 1;
        uint8_t queenSideCastleWhite: 1;
        uint8_t kingSideCastleWhite: 1;
    };
};

// For Move.flags
#define QUIETMOVE 0b0000
#define DOUBLEMOVE 0b0001
#define KINGCASTLE 0b0010
#define QUEENCASTLE 0b0011
#define CAPTUREMOVE 0b0100
#define ENPASSANTCAPTURE 0b0101
#define KNIGHTPROMO 0b1000
#define BISHOPPROMO 0b1001
#define ROOKPROMO 0b1010
#define QUEENPROMO 0b1011
#define KNIGHTPROMOCAPTURE 0b1100
#define BISHOPPROMOCAPTURE 0b1101
#define ROOKPROMOCAPTURE 0b1110
#define QUEENPROMOCAPTURE 0b1111

//e8
#define BLACKKINGSTARTSQUARE 60
//e1
#define WHITEKINGSTARTSQUARE 4

union Move{
    uint16_t raw;
    struct{
        uint16_t source : 6;
        uint16_t dest : 6;
        uint16_t flags : 4;
    };
};

struct MoveList{
    Move moves[MAXMOVES];
    int moveScores[MAXMOVES];
    int count;
};

struct BoardState {
    //pieces[Color][Type]
    Bitboard pieces[2][6];
    //0 white, 1 black, 2 combined
    Bitboard occupiedSquares[3];
    
    int sideToMove;
    // 0bxxxxKQkq
    CastlingRights castlingRights;
    int enPassantSquare;
    uint64_t zobristHash;
    int8_t pieceArray[64];
};

struct UndoState {
    int capturedPieceType;    // PAWN to QUEEN, or -1 if no capture
    int enPassantSquare;      // The EP square BEFORE the move was made
    CastlingRights castling;  // The castling rights BEFORE the move was made
    uint64_t oldZobristHash;
};

void fenToBoardState(const std::string& fen, BoardState& boardState);
void populateOccupiedSquares(BoardState& boardState);
void generateKnightAttacks();
void generateKingAttacks();
uint64_t generatePawnMoves(BoardState& boardState, int square, int color);
uint64_t generateDiagnalMoves(BoardState& boardState, int square, int color);
uint64_t generateStraightMoves(BoardState& boardState, int square, int color);
uint64_t generateBishopMoves(BoardState& boardState, int square, int color);
uint64_t generateRookMoves(BoardState& boardState, int square, int color);
uint64_t generateQueenMoves(BoardState& boardState, int square, int color);
MoveList generateMoves(BoardState& boardState, int color);
void makeMove(BoardState& boardState, Move move, UndoState& undoState);
void unmakeMove(BoardState& boardState, Move move, UndoState undoState);
bool isSquareAttacked(BoardState& boardState, int square, int attackerColor);
MoveList generateLegalMoves(BoardState boardState);
void perftDivide(BoardState& boardState, int depth);
uint64_t perft(BoardState& boardState, int depth);
std::string squareToAlgebraic(int sq);
Move strMoveToMove(const std::string& strMove, BoardState& boardState);
std::string boardStateToFen(BoardState& boardState);
void moveSwap(MoveList legalMoves, Move possibleBestMove);
std::string promotionChar(Move move);
std::string createPositionCmd(BoardState& boardState);

inline void setBit(Bitboard& bb, int square){bb |= 1ULL << square;}
inline void clearBit(Bitboard& bb, int square){bb &= ~(1ULL << square);}
inline bool isOccupied(Bitboard bb, int square){return (bb & (1ULL << square)) != 0;}

#endif
