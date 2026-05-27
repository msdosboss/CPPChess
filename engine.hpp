#ifndef ENGINE_HPP
#define ENGINE_HPP

#include <cstdint>
#include <string>
#include <iostream>

typedef uint64_t Bitboard;

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
#define QUITEMOVE 0b0000
#define DOUBLEMOVE 0b0001
#define KINGCASTLE 0b0010
#define QUEENCASTLE 0b0011
#define CAPTUREMAVE 0b0100
#define ENPASSANTCAPTURE 0b0101
#define KNIGHTPROMO 0b1000
#define BISHOPPROMO 0b1001
#define ROOKPROMO 0b1010
#define QUEENPROMO 0b1011
#define KNIGHTPROMOCAPTURE 0b1100
#define BISHOPPROMOCAPTURE 0b1101
#define ROOKPROMOCAPTURE 0b1110
#define QUEENPROMOCAPTURE 0b1111

union Move{
    uint16_t raw;
    struct{
        uint16_t source : 6;
        uint16_t destination : 6;
        uint16_t flags : 4;
    };
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
};

int fenSquareAdvance(int square, int n);
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

#endif
