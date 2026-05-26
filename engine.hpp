#ifndef ENGINE_HPP
#define ENGINE_HPP

#include <cstdint>
#include <string>

typedef uint64_t Bitboard;

#define WHITE 0
#define BLACK 1

#define PAWN 0
#define BISHOP 1
#define KNIGHT 2
#define ROOK 3
#define QUEEN 4
#define KING 5

struct BoardState {
    //pieces[Color][Type]
    Bitboard pieces[2][6];
    //0 white, 1 black, 2 combined
    Bitboard occupiedSquares[3];
    
    int sideToMove;
    int castlingRights;
    int enPassantSquare;
};

int fenSquareAdvance(int square, int n);
void fenToBoardState(std::string const fen, BoardState& boardState);
void populateOccupiedSquares(BoardState& boardState);

#endif
