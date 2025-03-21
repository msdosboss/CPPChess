#ifndef PIECE_H
#define PIECE_H
#include <iostream>
#include <cstdint>
#include <algorithm>
#include <unordered_map>
#include <vector>
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

class Board;
class Square;

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
		int file;
		int rank;
		Square *attackedSquares[27];	//27 is the most squares one piece can attack 

		Piece(uint8_t pieceIDCon, SDL_Texture *pieceImgCon, uint8_t statusFlagCon, int fileCon, int rankCon);
		Piece(int fileCon, int rankCon);	

		void squaresAttacked(Board *board);
		Move **pawnsAttackSquares(Board *board);
		Move **piecesLegalMoves(Board *board);
		
};

class Square{
	public:
		Piece *piece;
		SDL_Rect rect;

		Square();
		Square(int xPos, int yPos);
};

class Board{
	public:
		Piece *whiteKing;
		Piece *blackKing;
		Piece *whitePieces[17];	//max of 16 white pieces can be on board + 1 for NULL
		Piece *blackPieces[17];	//max of 16 black pieces can be on board + 1 for NULL	

		//std::unordered_map<Piece*, std::vector<Piece>> whitePieces
		//std::unordered_map<Piece*, std::vector<Piece>> blackPieces

		Square **squares;

		Board();

		void addPiece(Piece *pieceToAdd, uint8_t color);
		int isInCheck(uint8_t color, Square *targetSquare);
		void squaresAttackedInit();
		void updateAdjacentAttackSquares(Square *square);
		int move(Square *selectedSquare, Square *moveSquare, int color, uint8_t pawnPromotionChoice, SDL_Renderer *rend);
};

void freeLegalMoves(Move **legalMoves);
void slidingMoves(Board *board, Move **legalMoves, int selectedPieceFile, int selectedPieceRank, int ver, int hor);
void knightMoves(Board *board, Move **legalMoves, int selectedPieceFile, int selectedPieceRank);
void pawnMoves(Board *board, Move **legalMoves, int selectedPieceFile, int selectedPieceRank);
void kingMoves(Board *board, Move **legalMoves, int selectedPieceFile, int selectedPieceRank);
#endif
