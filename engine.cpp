#include "engine.hpp"

inline void setBit(Bitboard& bb, int square){bb |= 1ULL << square;}
inline void clearBit(Bitboard& bb, int square){bb &= ~(1ULL << square);}
inline bool isOccupied(Bitboard bb, int square){return (bb & (1ULL << square)) != 0;}

Bitboard knightAttacks[64];
Bitboard kingAttacks[64];
// Masks to prevent wrapping around the edges
const uint64_t fileA = 0x0101010101010101ULL;
const uint64_t fileB = 0x0202020202020202ULL;
const uint64_t fileG = 0x4040404040404040ULL;
const uint64_t fileH = 0x8080808080808080ULL;

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

void generateKingAttacks(){
    for(int i = 0; i < 64; i++){
        uint64_t b = 1ULL << i;
        uint64_t attacks = 0ULL;
        
        //up left
        attacks |= (b << 7) & ~fileH;
        //up
        attacks |= (b << 8);
        //up right
        attacks |= (b << 9) & ~fileA;
        //down left
        attacks |= (b << 9) & ~fileH;
        //down
        attacks |= (b << 8);
        //down right
        attacks |= (b << 7) & ~fileA;

        kingAttacks[i] = attacks;
    }
    
}

void generateKnightAttacks(){

    for(int i = 0; i < 64; i++){
        uint64_t b = 1ULL << i;
        uint64_t attacks = 0ULL;

        //up 2 right 1
        attacks |= (b << 17) & ~fileA;
        //up 2 left 1
        attacks |= (b << 15) & ~fileH;
        //up 1 right 2
        attacks |= (b << 10) & ~(fileA | fileB);
        //up 1 left 2
        attacks |= (b << 6) & ~(fileG | fileH);
        //down 2 right 1
        attacks |= (b >> 15) & ~fileA;
        //down 2 left 1
        attacks |= (b >> 17) & ~fileH;
        //down 1 right 2
        attacks |= (b >> 6) & ~(fileA | fileB);
        //down 1 left 2
        attacks |= (b >> 10) & ~(fileG | fileH);

        knightAttacks[i] = attacks;
    }
}

uint64_t generatePawnMoves(BoardState& boardState, int square, int color){
    uint64_t b = 1ULL << square;
    uint64_t squaresAttacked = 0ULL;
    if(color == WHITE){
        //Moving forward
        squaresAttacked |= (b << 8) & ~boardState.occupiedSquares[2]; 
        if(((b << 8) & ~boardState.occupiedSquares[2]) != 0){
            squaresAttacked |= (b << 16) & ~(boardState.occupiedSquares[2]);
        }
        //capture
        squaresAttacked |= ((b << 9) & ~fileA) & boardState.occupiedSquares[BLACK];
        squaresAttacked |= ((b << 7) & ~fileH) & boardState.occupiedSquares[BLACK];
    }
    else{
        //Moving forward
        squaresAttacked |= (b >> 8) & ~boardState.occupiedSquares[2]; 
        if(((b >> 8) & ~boardState.occupiedSquares[2]) != 0){
            squaresAttacked |= (b >> 16) & ~(boardState.occupiedSquares[2]);
        }
        //capture
        squaresAttacked |= ((b >> 9) & ~fileH) & boardState.occupiedSquares[WHITE];
        squaresAttacked |= ((b >> 7) & ~fileA) & boardState.occupiedSquares[WHITE];
    }
    return squaresAttacked;
}


uint64_t generateStraightMoves(BoardState& boardState, int square, int color){
    uint64_t b = 1ULL << square;
    uint64_t squaresAttacked = 0ULL;

    int i = 1;
    //Up
    while(((b << (8 * i)) & ~boardState.occupiedSquares[color]) != 0){
        squaresAttacked |= b << (8 * i);
        //collides into opponents pieces
        if((b << (8 * i)) & ~boardState.occupiedSquares[2]){
            break;
        }
        i++;
    }
    i = 1;
    //Down
    while(((b >> (8 * i)) & ~boardState.occupiedSquares[color]) != 0){
        squaresAttacked |= b >> (8 * i);
        //collides into opponents pieces
        if((b >> (8 * i)) & ~boardState.occupiedSquares[2]){
            break;
        }
        i++;
    }
    i = 1;
    //Right
    while(((b << i) & ~(boardState.occupiedSquares[color] | fileA)) != 0){
        squaresAttacked |= b << i;
        //collides into opponents pieces
        if((b << i) & ~boardState.occupiedSquares[2]){
            break;
        }
        i++;
    }
    i = 1;
    //Left
    while(((b >> i) & ~(boardState.occupiedSquares[color] | fileH)) != 0){
        squaresAttacked |= b >> i;
        //collides into opponents pieces
        if((b >> i) & ~boardState.occupiedSquares[2]){
            break;
        }
        i++;
    }

    return squaresAttacked;
}

uint64_t generateDiagnalMoves(BoardState& boardState, int square, int color){
    uint64_t b = 1ULL << square;
    uint64_t squaresAttacked = 0ULL;

    int i = 1;
    //Up and left
    while(((b << (7 * i)) & ~(boardState.occupiedSquares[color] | fileH)) != 0){
        squaresAttacked |= b << (7 * i);
        //collides into opponents pieces
        if((b << (7 * i)) & ~boardState.occupiedSquares[2]){
            break;
        }
        i++;
    }
    i = 1;
    //Up and right
    while(((b << (9 * i)) & ~(boardState.occupiedSquares[color] | fileA)) != 0){
        squaresAttacked |= b << (9 * i);
        //collides into opponents pieces
        if((b << (9 * i)) & ~boardState.occupiedSquares[2]){
            break;
        }
        i++;
    }
    i = 1;
    //Down and right
    while(((b >> (7 * i)) & ~(boardState.occupiedSquares[color] | fileA)) != 0){
        squaresAttacked |= b >> (7 * i);
        //collides into opponents pieces
        if((b >> (7 * i)) & ~boardState.occupiedSquares[2]){
            break;
        }
        i++;
    }
    i = 1;
    //Down and left
    while(((b >> (9 * i)) & ~(boardState.occupiedSquares[color] | fileH)) != 0){
        squaresAttacked |= b >> (9 * i);
        //collides into opponents pieces
        if((b >> (9 * i)) & ~boardState.occupiedSquares[2]){
            break;
        }
        i++;
    }

    return squaresAttacked;
}

uint64_t generateBishopMoves(BoardState& boardState, int square, int color){
    uint64_t squaresAttacked = generateDiagnalMoves(boardState, square, color);
    return squaresAttacked;
}

uint64_t generateRookMoves(BoardState& boardState, int square, int color){
    uint64_t squaresAttacked = generateStraightMoves(boardState, square, color);
    return squaresAttacked;
}

uint64_t generateQueenMoves(BoardState& boardState, int square, int color){
    uint64_t squaresAttacked = generateDiagnalMoves(boardState, square, color);
    squaresAttacked |= generateStraightMoves(boardState, square, color);
    return squaresAttacked;
}

uint64_t generateKnightMoves(BoardState& boardState, int square, int color){
    uint64_t squaresAttacked = knightAttacks[square] & ~boardState.occupiedSquares[color];

    return squaresAttacked;
    
}

uint64_t generateKingMoves(BoardState& boardState, int square, int color){
    uint64_t squaresAttacked = kingAttacks[square] & ~boardState.occupiedSquares[color];

    return squaresAttacked;
}



int fenSquareAdvance(int square, int n){
    for(int i = 0; i < n; i++){
        //Changing rows
        if((square + 1) % 8 == 7){
            square -= 15;
        }
        else{
            square++;
        }
    }

    return square;
}

void fenToBoardState(const std::string& fen, BoardState& boardState){
    for(int i = 0; i < 6; i++){
        boardState.pieces[WHITE][i] = 0ULL;
        boardState.pieces[BLACK][i] = 0ULL;
    }
    //56 is a8
    int square = 56;
    int i = 0;
    while(fen[i] != ' '){ 
        if(fen[i] == '/'){
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
        i++;
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
    //Skip space
    i++;
    boardState.castlingRights.raw = 0;
    while(fen[i] != ' '){
        switch(fen[i]){
            case 'K':
                boardState.castlingRights.kingSideCastleWhite = 1;
                break;
            case 'Q':
                boardState.castlingRights.queenSideCastleWhite = 1;
                break;
            case 'k':
                boardState.castlingRights.kingSideCastleBlack = 1;
                break;
            case 'q':
                boardState.castlingRights.queenSideCastleBlack = 1;
                break;
        }
    }
    //Skip space
    i++;
    if(fen[i] == '-'){
        i++;
        boardState.enPassantSquare = -1;
    }
    else{
        // Square in form of "e4" 
        // 'a' - 97 = 0, '1' - 49 = 0
        boardState.enPassantSquare = (fen[i] - 97) + (fen[i + 1] - 49) * 8;
        i += 2;
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
