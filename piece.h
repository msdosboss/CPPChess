#ifndef PIECE_H
#define PIECE_H
#include <iostream>
#include <cstdint>
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

#define PAWNTWOMOVEFLAG 0b1
#define PAWNENPASFLAG 0b10

class Piece {
	public:
		uint8_t pieceID;
		SDL_Texture *pieceImg;
		uint8_t statusFlag;	//will be used for things like castle and en pasente
		SDL_Rect rect;
	
		Piece(int xPos, int yPos);
		Piece(uint8_t pieceIDCon, SDL_Texture *pieceImgCon, uint8_t statusFlagCon, int xPos, int yPos);

		Piece **piecesLegalMoves(Piece ***board);
		int move(Piece ***board, Piece *moveSquare, int color);

		
};

void slidingMoves(Piece ***board, Piece **legalMoves, int selectedPieceFile, int selectedPieceRank, int ver, int hor);
void knightMoves(Piece ***board, Piece **legalMoves, int selectedPieceFile, int selectedPieceRank);
#endif
