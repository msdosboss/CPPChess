#include "evaluate.hpp"
int pieceValue[6] = {
    100,  //Pawn
    300,  //Bishop
    300,  //Knight
    500,  //Rook
    900,  //Queen
    20000 //King
};

int whitePawnSquareTable[64] = {
   //A    B    C    D    E    F    G    H 
      0,   0,   0,   0,   0,   0,   0,   0, //1
      0,   0,   0,   0,   0,   0,   0,   0, //2
      0,   0,   0,   0,   0,   0,   0,   0, //3
      0,   0,   0,   0,   0,   0,   0,   0, //4
      0,   0,   0,   0,   0,   0,   0,   0, //5
      0,   0,   0,   0,   0,   0,   0,   0, //6
      0,   0,   0,   0,   0,   0,   0,   0, //7
      0,   0,   0,   0,   0,   0,   0,   0  //8
};

int blackPawnSquareTable[64] = {
   //A    B    C    D    E    F    G    H 
      0,   0,   0,   0,   0,   0,   0,   0, //1
      0,   0,   0,   0,   0,   0,   0,   0, //2
      0,   0,   0,   0,   0,   0,   0,   0, //3
      0,   0,   0,   0,   0,   0,   0,   0, //4
      0,   0,   0,   0,   0,   0,   0,   0, //5
      0,   0,   0,   0,   0,   0,   0,   0, //6
      0,   0,   0,   0,   0,   0,   0,   0, //7
      0,   0,   0,   0,   0,   0,   0,   0  //8
};

int whiteBishopSquareTable[64] = {
   //A    B    C    D    E    F    G    H 
      0,   0,   0,   0,   0,   0,   0,   0, //1
      0,   0,   0,   0,   0,   0,   0,   0, //2
      0,   0,   0,   0,   0,   0,   0,   0, //3
      0,   0,   0,   0,   0,   0,   0,   0, //4
      0,   0,   0,   0,   0,   0,   0,   0, //5
      0,   0,   0,   0,   0,   0,   0,   0, //6
      0,   0,   0,   0,   0,   0,   0,   0, //7
      0,   0,   0,   0,   0,   0,   0,   0  //8
};

int blackBishopSquareTable[64] = {
   //A    B    C    D    E    F    G    H 
      0,   0,   0,   0,   0,   0,   0,   0, //1
      0,   0,   0,   0,   0,   0,   0,   0, //2
      0,   0,   0,   0,   0,   0,   0,   0, //3
      0,   0,   0,   0,   0,   0,   0,   0, //4
      0,   0,   0,   0,   0,   0,   0,   0, //5
      0,   0,   0,   0,   0,   0,   0,   0, //6
      0,   0,   0,   0,   0,   0,   0,   0, //7
      0,   0,   0,   0,   0,   0,   0,   0  //8
};

//Index 0 a1, index 63 h8
//Meaning top left is a1 and bottom right h8
int whiteKnightSquareTable[64] = {
   //A    B    C    D    E    F    G    H 
    -50, -40, -30, -30, -30, -30, -40, -50, //1
    -40, -20,   0,   5,   5,   0, -20, -40, //2
    -30,   5,  10,  15,  15,  10,   5, -30, //3
    -30,   0,  15,  20,  20,  15,   0, -30, //4
    -30,   5,  15,  20,  20,  15,   5, -30, //5
    -30,   0,  10,  15,  15,  10,   0, -30, //6
    -40, -20,   0,   0,   0,   0, -20, -40, //7
    -50, -40, -30, -30, -30, -30, -40, -50  //8
};

int blackKnightSquareTable[64] = {
   //A    B    C    D    E    F    G    H 
    -50, -40, -30, -30, -30, -30, -40, -50, //1
    -40, -20,   0,   0,   0,   0, -20, -40, //2
    -30,   0,  10,  15,  15,  10,   0, -30, //3
    -30,   5,  15,  20,  20,  15,   5, -30, //4
    -30,   0,  15,  20,  20,  15,   0, -30, //5
    -30,   5,  10,  15,  15,  10,   5, -30, //6
    -40, -20,   0,   5,   5,   0, -20, -40, //7
    -50, -40, -30, -30, -30, -30, -40, -50  //8
};

int whiteRookSquareTable[64] = {
   //A    B    C    D    E    F    G    H 
      0,   0,   0,   0,   0,   0,   0,   0, //1
      0,   0,   0,   0,   0,   0,   0,   0, //2
      0,   0,   0,   0,   0,   0,   0,   0, //3
      0,   0,   0,   0,   0,   0,   0,   0, //4
      0,   0,   0,   0,   0,   0,   0,   0, //5
      0,   0,   0,   0,   0,   0,   0,   0, //6
      0,   0,   0,   0,   0,   0,   0,   0, //7
      0,   0,   0,   0,   0,   0,   0,   0  //8
};

int blackRookSquareTable[64] = {
   //A    B    C    D    E    F    G    H 
      0,   0,   0,   0,   0,   0,   0,   0, //1
      0,   0,   0,   0,   0,   0,   0,   0, //2
      0,   0,   0,   0,   0,   0,   0,   0, //3
      0,   0,   0,   0,   0,   0,   0,   0, //4
      0,   0,   0,   0,   0,   0,   0,   0, //5
      0,   0,   0,   0,   0,   0,   0,   0, //6
      0,   0,   0,   0,   0,   0,   0,   0, //7
      0,   0,   0,   0,   0,   0,   0,   0  //8
};

int whiteQueenSquareTable[64] = {
   //A    B    C    D    E    F    G    H 
      0,   0,   0,   0,   0,   0,   0,   0, //1
      0,   0,   0,   0,   0,   0,   0,   0, //2
      0,   0,   0,   0,   0,   0,   0,   0, //3
      0,   0,   0,   0,   0,   0,   0,   0, //4
      0,   0,   0,   0,   0,   0,   0,   0, //5
      0,   0,   0,   0,   0,   0,   0,   0, //6
      0,   0,   0,   0,   0,   0,   0,   0, //7
      0,   0,   0,   0,   0,   0,   0,   0  //8
};

int blackQueenSquareTable[64] = {
   //A    B    C    D    E    F    G    H 
      0,   0,   0,   0,   0,   0,   0,   0, //1
      0,   0,   0,   0,   0,   0,   0,   0, //2
      0,   0,   0,   0,   0,   0,   0,   0, //3
      0,   0,   0,   0,   0,   0,   0,   0, //4
      0,   0,   0,   0,   0,   0,   0,   0, //5
      0,   0,   0,   0,   0,   0,   0,   0, //6
      0,   0,   0,   0,   0,   0,   0,   0, //7
      0,   0,   0,   0,   0,   0,   0,   0  //8
};

int whiteKingSquareTable[64] = {
   //A    B    C    D    E    F    G    H 
      0,   0,   0,   0,   0,   0,   0,   0, //1
      0,   0,   0,   0,   0,   0,   0,   0, //2
      0,   0,   0,   0,   0,   0,   0,   0, //3
      0,   0,   0,   0,   0,   0,   0,   0, //4
      0,   0,   0,   0,   0,   0,   0,   0, //5
      0,   0,   0,   0,   0,   0,   0,   0, //6
      0,   0,   0,   0,   0,   0,   0,   0, //7
      0,   0,   0,   0,   0,   0,   0,   0  //8
};

int blackKingSquareTable[64] = {
   //A    B    C    D    E    F    G    H 
      0,   0,   0,   0,   0,   0,   0,   0, //1
      0,   0,   0,   0,   0,   0,   0,   0, //2
      0,   0,   0,   0,   0,   0,   0,   0, //3
      0,   0,   0,   0,   0,   0,   0,   0, //4
      0,   0,   0,   0,   0,   0,   0,   0, //5
      0,   0,   0,   0,   0,   0,   0,   0, //6
      0,   0,   0,   0,   0,   0,   0,   0, //7
      0,   0,   0,   0,   0,   0,   0,   0  //8
};

//2 colors 6 pieces and 64 squares
int *squareTables[2][6] = {
    {
        whitePawnSquareTable, 
        whiteBishopSquareTable, 
        whiteKnightSquareTable,
        whiteRookSquareTable,
        whiteQueenSquareTable,
        whiteKingSquareTable
    },
    {
        blackPawnSquareTable,
        blackBishopSquareTable,
        blackKnightSquareTable,
        blackRookSquareTable,
        blackQueenSquareTable,
        blackKingSquareTable
    }
};

int evaluate(BoardState& boardState){
    int scores[2] = {0, 0};

    for(int i = PAWN; i <= KING; i++){
        scores[WHITE] += __builtin_popcountll(boardState.pieces[WHITE][i]) * pieceValue[i];
        scores[BLACK] += __builtin_popcountll(boardState.pieces[BLACK][i]) * pieceValue[i];
    }

    for(int i = PAWN; i <= KING; i++){
        Bitboard whitebb = boardState.pieces[WHITE][i];
        Bitboard blackbb = boardState.pieces[BLACK][i];
        while(whitebb != 0){
            int pieceIndex = __builtin_ctzll(whitebb);
            scores[WHITE] += squareTables[WHITE][i][pieceIndex];
            clearBit(whitebb, pieceIndex);
        }
        while(blackbb != 0){
            int pieceIndex = __builtin_ctzll(blackbb);
            scores[BLACK] += squareTables[BLACK][i][pieceIndex];
            clearBit(blackbb, pieceIndex);
        }
    }

    return scores[WHITE] - scores[BLACK];
}
