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
      5,  10,  10, -20, -20,  10,  10,   5, //2
      5,  -5, -10,   0,   0, -10,  -5,   5, //3
      0,   0,   0,  20,  20,   0,   0,   0, //4
      5,   5,  10,  25,  25,  10,   5,   5, //5
     10,  10,  20,  30,  30,  20,  10,  10, //6
     50,  50,  50,  50,  50,  50,  50,  50, //7
      0,   0,   0,   0,   0,   0,   0,   0  //8
};

int blackPawnSquareTable[64] = {
   //A    B    C    D    E    F    G    H 
      0,   0,   0,   0,   0,   0,   0,   0, //1
     50,  50,  50,  50,  50,  50,  50,  50, //2
     10,  10,  20,  30,  30,  20,  10,  10, //3
      5,   5,  10,  25,  25,  10,   5,   5, //4
      0,   0,   0,  20,  20,   0,   0,   0, //5
      5,  -5, -10,   0,   0, -10,  -5,   5, //6
      5,  10,  10, -20, -20,  10,  10,   5, //7
      0,   0,   0,   0,   0,   0,   0,   0  //8
};

int whiteBishopSquareTable[64] = {
   //A    B    C    D    E    F    G    H 
    -20, -10, -10, -10, -10, -10, -10, -20, //1
    -10,   5,   0,   0,   0,   0,   5, -10, //2
    -10,  10,  10,  10,  10,  10,  10, -10, //3
    -10,   0,  10,  10,  10,  10,   0, -10, //4
    -10,   5,   5,  10,  10,   5,   5, -10, //5
    -10,   0,   5,  10,  10,   5,   0, -10, //6
    -10,   0,   0,   0,   0,   0,   0, -10, //7
    -20, -10, -10, -10, -10, -10, -10, -20  //8
};

int blackBishopSquareTable[64] = {
   //A    B    C    D    E    F    G    H 
    -20, -10, -10, -10, -10, -10, -10, -20, //1
    -10,   0,   0,   0,   0,   0,   0, -10, //2
    -10,   0,   5,  10,  10,   5,   0, -10, //3
    -10,   5,   5,  10,  10,   5,   5, -10, //4
    -10,   0,  10,  10,  10,  10,   0, -10, //5
    -10,  10,  10,  10,  10,  10,  10, -10, //6
    -10,   5,   0,   0,   0,   0,   5, -10, //7
    -20, -10, -10, -10, -10, -10, -10, -20  //8
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
      0,   0,   0,   5,   5,   0,   0,   0, //1
     -5,   0,   0,   0,   0,   0,   0,  -5, //2
     -5,   0,   0,   0,   0,   0,   0,  -5, //3
     -5,   0,   0,   0,   0,   0,   0,  -5, //4
     -5,   0,   0,   0,   0,   0,   0,  -5, //5
     -5,   0,   0,   0,   0,   0,   0,  -5, //6
      5,  10,  10,  10,  10,  10,  10,   5, //7
      0,   0,   0,   0,   0,   0,   0,   0  //8
};

int blackRookSquareTable[64] = {
   //A    B    C    D    E    F    G    H 
      0,   0,   0,   0,   0,   0,   0,   0, //1
      5,  10,  10,  10,  10,  10,  10,   5, //2
     -5,   0,   0,   0,   0,   0,   0,  -5, //3
     -5,   0,   0,   0,   0,   0,   0,  -5, //4
     -5,   0,   0,   0,   0,   0,   0,  -5, //5
     -5,   0,   0,   0,   0,   0,   0,  -5, //6
     -5,   0,   0,   0,   0,   0,   0,  -5, //7
      0,   0,   0,   5,   5,   0,   0,   0  //8
};

int whiteQueenSquareTable[64] = {
   //A    B    C    D    E    F    G    H 
    -20, -10, -10,  -5,  -5, -10, -10, -20, //1
    -10,   0,   5,   0,   0,   0,   0, -10, //2
    -10,   5,   5,   5,   5,   5,   0, -10, //3
      0,   0,   5,   5,   5,   5,   0,  -5, //4
     -5,   0,   5,   5,   5,   5,   0,  -5, //5
    -10,   0,   5,   5,   5,   5,   0, -10, //6
    -10,   0,   0,   0,   0,   0,   0, -10, //7
    -20, -10, -10,  -5,  -5, -10, -10, -20  //8
};

int blackQueenSquareTable[64] = {
   //A    B    C    D    E    F    G    H 
    -20, -10, -10,  -5,  -5, -10, -10, -20, //1
    -10,   0,   0,   0,   0,   0,   0, -10, //2
    -10,   0,   5,   5,   5,   5,   0, -10, //3
     -5,   0,   5,   5,   5,   5,   0,  -5, //4
      0,   0,   5,   5,   5,   5,   0,  -5, //5
    -10,   5,   5,   5,   5,   5,   0, -10, //6
    -10,   0,   5,   0,   0,   0,   0, -10, //7
    -20, -10, -10,  -5,  -5, -10, -10, -20  //8
};

int whiteKingSquareTable[64] = {
   //A    B    C    D    E    F    G    H 
     20,  30,  10,   0,   0,  10,  30,  20, //1
     20,  20,   0,   0,   0,   0,  20,  20, //2
    -10, -20, -20, -20, -20, -20, -20, -10, //3
    -20, -30, -30, -40, -40, -30, -30, -20, //4
    -30, -40, -40, -50, -50, -40, -40, -30, //5
    -30, -40, -40, -50, -50, -40, -40, -30, //6
    -30, -40, -40, -50, -50, -40, -40, -30, //7
    -30, -40, -40, -50, -50, -40, -40, -30  //8
};

int blackKingSquareTable[64] = {
   //A    B    C    D    E    F    G    H 
    -30, -40, -40, -50, -50, -40, -40, -30, //1
    -30, -40, -40, -50, -50, -40, -40, -30, //2
    -30, -40, -40, -50, -50, -40, -40, -30, //3
    -30, -40, -40, -50, -50, -40, -40, -30, //4
    -20, -30, -30, -40, -40, -30, -30, -20, //5
    -10, -20, -20, -20, -20, -20, -20, -10, //6
     20,  20,   0,   0,   0,   0,  20,  20, //7
     20,  30,  10,   0,   0,  10,  30,  20  //8
};

int whiteKingSquareTableEndGame[64] = {
   //A    B    C    D    E    F    G    H 
    -30, -40, -40, -50, -50, -40, -40, -30, //1
    -30, -40, -40, -50, -50, -40, -40, -30, //2
    -30, -40, -40, -50, -50, -40, -40, -30, //3
    -30, -40, -40, -50, -50, -40, -40, -30, //4
    -20, -30, -30, -40, -40, -30, -30, -20, //5
    -10, -20, -20, -20, -20, -20, -20, -10, //6
     20,  20,   0,   0,   0,   0,  20,  20, //7
     20,  30,  10,   0,   0,  10,  30,  20  //8
};
int blackKingSquareTableEndGame[64] = {
   //A    B    C    D    E    F    G    H 
    -30, -40, -40, -50, -50, -40, -40, -30, //1
    -30, -40, -40, -50, -50, -40, -40, -30, //2
    -30, -40, -40, -50, -50, -40, -40, -30, //3
    -30, -40, -40, -50, -50, -40, -40, -30, //4
    -20, -30, -30, -40, -40, -30, -30, -20, //5
    -10, -20, -20, -20, -20, -20, -20, -10, //6
     20,  20,   0,   0,   0,   0,  20,  20, //7
     20,  30,  10,   0,   0,  10,  30,  20  //8
};

//2 colors 6 pieces and 64 squares
int *squareTablesMidGame[2][6] = {
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

int *squareTablesEndGame[2][6] = {
    {
        whitePawnSquareTable, 
        whiteBishopSquareTable, 
        whiteKnightSquareTable,
        whiteRookSquareTable,
        whiteQueenSquareTable,
        whiteKingSquareTableEndGame
    },
    {
        blackPawnSquareTable,
        blackBishopSquareTable,
        blackKnightSquareTable,
        blackRookSquareTable,
        blackQueenSquareTable,
        blackKingSquareTableEndGame
    }
};

int evaluate(BoardState& boardState){
    int scores[2] = {0, 0};

    for(int i = PAWN; i <= KING; i++){
        scores[WHITE] += __builtin_popcountll(boardState.pieces[WHITE][i]) * pieceValue[i];
        scores[BLACK] += __builtin_popcountll(boardState.pieces[BLACK][i]) * pieceValue[i];
    }
    int ***squareTables;
    //41700 20000 + 20000 = 40000 both kings 1700 is 17 pieces of material between both sides
    //So I call that endgame
    if(scores[WHITE] + scores[BLACK] < 41700){
        squareTables = (int***)squareTablesEndGame;
    }
    else{
        squareTables = (int***)squareTablesMidGame;
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
