#include "engine.hpp"

inline void setBit(Bitboard& bb, int square){bb |= 1ULL << square;}
inline void clearBit(Bitboard& bb, int square){bb &= ~(1ULL << square);}
inline bool isOccupied(Bitboard bb, int square){return (bb & (1ULL << square)) != 0;}

/*
  a  b  c  d  e  f  g  h 
8 56 57 58 59 60 61 62 63
7 48 49 50 51 52 53 54 55
6 40 41 42 43 44 45 46 47
5 32 33 34 35 36 37 38 39
4 24 25 26 27 28 29 30 31
3 16 17 18 19 20 21 22 23
2 8  9  10 11 12 13 14 15
1 0  1  2  3  4  5  6  7
*/
int fenSquareAdvance(int square, int n){
    for(int i = 0; i < n; i++){
        //Changing rows
        if((square + 1) % 8 == 0){
            square -= 15;
        }
        else{
            square++;
        }
    }

    return square;
}

void fenToBoardState(std::string const fen, BoardState& boardState){
    for(int i = 0; i < 6; i++){
        boardState.pieces[WHITE][i] = 0ULL;
        boardState.pieces[BLACK][i] = 0ULL;
    }
    //56 is a8
    int square = 56;
    int i = 0;
    while(fen[i] != ' '){ 
        if(fen[i] == '\\'){
            i++;
            square = fenSquareAdvance(square, fen[i] - 48);
            continue;
        }
        switch (fen[i]){
            case 'r':
                setBit(boardState.pieces[BLACK][ROOK], square);
                break;
            case 'b':
                setBit(boardState.pieces[BLACK][ROOK], square);
                break;
            case 'n':
                setBit(boardState.pieces[BLACK][KNIGHT], square);
                break;
            case 'q':
                setBit(boardState.pieces[BLACK][QUEEN], square);
                break;
            case 'k':
                setBit(boardState.pieces[BLACK][KING], square);
                break;
            case 'p':
                setBit(boardState.pieces[BLACK][PAWN], square);
                break;
            case 'R':
                setBit(boardState.pieces[WHITE][ROOK], square);
                break;
            case 'B':
                setBit(boardState.pieces[WHITE][BISHOP], square);
                break;
            case 'N':
                setBit(boardState.pieces[WHITE][KNIGHT], square);
                break;
            case 'Q':
                setBit(boardState.pieces[WHITE][QUEEN], square);
                break;
            case 'K':
                setBit(boardState.pieces[WHITE][KING], square);
                break;
            case 'P':
                setBit(boardState.pieces[WHITE][PAWN], square);
                break;
                
        }
        square = fenSquareAdvance(square, 1);
    }
    populateOccupiedSquares(boardState);
    //Skip space
    i++;
    if(fen[i] == 'w'){
        boardState.sideToMove = WHITE;
    }
    else{
        boardState.sideToMove = BLACK;
    }

}

void populateOccupiedSquares(BoardState& boardState){
    for(int i = 0; i < 2; i++){
        boardState.occupiedSquares[i] = 0ULL;
    }
    for(int i = 0; i < 6; i++){
        boardState.occupiedSquares[WHITE] |= boardState.pieces[WHITE][i];
        boardState.occupiedSquares[BLACK] |= boardState.pieces[BLACK][i];
        boardState.occupiedSquares[2] = boardState.occupiedSquares[WHITE] | boardState.occupiedSquares[BLACK]; 
    }
}
