#ifndef PIECE_H
#define PIECE_H
#include <iostream>
#include <cstdint>
#include <algorithm>
#include <SDL.h>
#include <SDL_image.h>

#define SIZE 100

#define PAWN   0b00000001
#define KNIGHT 0b00000010
#define BISHOP 0b00000011
#define ROOK   0b00000100
#define QUEEN  0b00000101
#define KING   0b00000111

#define WHITE  0b00001000
#define BLACK  0b00010000

#define CANCASTLE 0b1
#define PAWNENPASFLAG 0b10
#define PAWNPROMOQUEENFLAG 0b100
#define PAWNPROMOROOKFLAG 0b1000
#define PAWNPROMOBISHOPFLAG 0b10000
#define PAWNPROMOKNIGHTFLAG 0b100000
#define KINGSIDECASTLE 0b1000000
#define QUEENSIDECASTLE 0b10000000

class Move{
	public:
		int file;
		int rank;
		uint8_t statusFlag;

		Move(int fileCon, int rankCon, uint8_t statusFlagCon);
};

class Piece {
	public:
		uint8_t pieceID;
		SDL_Texture *pieceImg;
		uint8_t statusFlag;	//will be used for things like castle and en pasente
		SDL_Rect rect;
		Piece[27] *attackedSquares;	//27 is the most squares one piece can attack 
	
		Piece(int xPos, int yPos);
		Piece(uint8_t pieceIDCon, SDL_Texture *pieceImgCon, uint8_t statusFlagCon, int xPos, int yPos);

		void squaresAttacked(Piece ***board);
		Move **piecesLegalMoves(Piece ***board);
		int move(Piece ***board, Piece *moveSquare, int color, uint8_t pawnPromotionChoice, SDL_Renderer *rend);

		
};

void freeLegalMoves(Move **legalMoves);
void slidingMoves(Piece ***board, Move **legalMoves, int selectedPieceFile, int selectedPieceRank, int ver, int hor);
void knightMoves(Piece ***board, Move **legalMoves, int selectedPieceFile, int selectedPieceRank);
void pawnMoves(Piece ***board, Move **legalMoves, int selectedPieceFile, int selectedPieceRank);
void kingMoves(Piece ***board, Move **legalMoves, int selectedPieceFile, int selectedPieceRank);
#endif
