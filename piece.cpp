#include "piece.h"

Move::Move(int fileCon, int rankCon, uint8_t statusFlagCon){
	file = fileCon;
	rank = rankCon;
	statusFlag = statusFlagCon;	
}

void freeLegalMoves(Move **legalMoves){
	int legalMovesIndex = 0;
	while(legalMoves[legalMovesIndex] != NULL){
		delete legalMoves[legalMovesIndex++];
	}
}

Piece::Piece(int xPos, int yPos){
	pieceID = 0;
	pieceImg = NULL;
	statusFlag = 0;
	rect = {(int) xPos, (int) yPos, SIZE, SIZE};
}


Piece::Piece(uint8_t pieceIDCon, SDL_Texture *pieceImgCon, uint8_t statusFlagCon, int xPos, int yPos){
	pieceID = pieceIDCon;
	pieceImg = pieceImgCon;
	statusFlag = statusFlagCon;	
	rect = {(int) xPos, (int) yPos, SIZE, SIZE};
}

void slidingMoves(Piece ***board, Move **legalMoves, int selectedPieceFile, int selectedPieceRank, int diagonal, int straight){
	int legalMovesIndex = 0;
	if(diagonal){
		for(int i = 1; (i + selectedPieceFile < 8 && i + selectedPieceRank < 8); i++){
				if(board[selectedPieceFile + i][selectedPieceRank + i]->pieceID == 0){
					legalMoves[legalMovesIndex++] = new Move(selectedPieceFile + i, selectedPieceRank + i, 0);
				}
				else if(((board[selectedPieceFile + i][selectedPieceRank + i]->pieceID & 0b11000) & (board[selectedPieceFile][selectedPieceRank]->pieceID & 0b11000)) == 0){	//meaing that they are different colors
					legalMoves[legalMovesIndex++] = new Move(selectedPieceFile + i, selectedPieceRank + i, 0);
					break;
				}
				else{
					break;
				}
		}

		for(int i = 1; (selectedPieceFile - i >= 0 && i + selectedPieceRank < 8); i++){
				if(board[selectedPieceFile - i][selectedPieceRank + i]->pieceID == 0){
					legalMoves[legalMovesIndex++] = new Move(selectedPieceFile - i, selectedPieceRank + i, 0);
				}
				else if(((board[selectedPieceFile - i][selectedPieceRank + i]->pieceID & 0b11000) & (board[selectedPieceFile][selectedPieceRank]->pieceID & 0b11000)) == 0){	//meaing that they are different colors
					legalMoves[legalMovesIndex++] = new Move(selectedPieceFile - i, selectedPieceRank + i, 0);
					break;
				}
				else{
					break;
				}
		}

		for(int i = 1; (i + selectedPieceFile < 8 && selectedPieceRank - i >= 0); i++){
				if(board[selectedPieceFile + i][selectedPieceRank - i]->pieceID == 0){
					legalMoves[legalMovesIndex++] = new Move(selectedPieceFile + i, selectedPieceRank - i, 0);
				}
				else if(((board[selectedPieceFile + i][selectedPieceRank - i]->pieceID & 0b11000) & (board[selectedPieceFile][selectedPieceRank]->pieceID & 0b11000)) == 0){	//meaing that they are different colors
					legalMoves[legalMovesIndex++] = new Move(selectedPieceFile + i, selectedPieceRank - i, 0);
					break;
				}
				else{
					break;
				}
		}

		for(int i = 1; (selectedPieceFile - i >= 0 && selectedPieceRank - i >= 0); i++){
				if(board[selectedPieceFile - i][selectedPieceRank - i]->pieceID == 0){
					legalMoves[legalMovesIndex++] = new Move(selectedPieceFile - i, selectedPieceRank - i, 0);
				}
				else if(((board[selectedPieceFile - i][selectedPieceRank - i]->pieceID & 0b11000) & (board[selectedPieceFile][selectedPieceRank]->pieceID & 0b11000)) == 0){	//meaing that they are different colors
					legalMoves[legalMovesIndex++] = new Move(selectedPieceFile - i, selectedPieceRank - i, 0);
					break;
				}
				else{
					break;
				}
		}

	}

	if(straight){
		for(int i = 1; selectedPieceFile + i < 8; i++){
			if(board[selectedPieceFile + i][selectedPieceRank]->pieceID == 0){
				legalMoves[legalMovesIndex++] = new Move(selectedPieceFile  + i, selectedPieceRank, 0);
			}
			else if(((board[selectedPieceFile + i][selectedPieceRank]->pieceID & 0b11000) & (board[selectedPieceFile][selectedPieceRank]->pieceID & 0b11000)) == 0){	//meaing that they are different colors
				legalMoves[legalMovesIndex++] = new Move(selectedPieceFile  + i, selectedPieceRank, 0);
				break;
			}
			else{
				break;
			}
		}

		for(int i = 1; selectedPieceFile - i >= 0; i++){
			if(board[selectedPieceFile - i][selectedPieceRank]->pieceID == 0){
				legalMoves[legalMovesIndex++] = new Move(selectedPieceFile - i, selectedPieceRank, 0);
			}
			else if(((board[selectedPieceFile - i][selectedPieceRank]->pieceID & 0b11000) & (board[selectedPieceFile][selectedPieceRank]->pieceID & 0b11000)) == 0){	//meaing that they are different colors
				legalMoves[legalMovesIndex++] = new Move(selectedPieceFile - i, selectedPieceRank, 0);
				break;
			}
			else{
				break;
			}
		}

		for(int i = 1; selectedPieceRank + i < 8; i++){
			if(board[selectedPieceFile][selectedPieceRank + i]->pieceID == 0){
				legalMoves[legalMovesIndex++] = new Move(selectedPieceFile, selectedPieceRank + i, 0);
			}
			else if(((board[selectedPieceFile][selectedPieceRank + i]->pieceID & 0b11000) & (board[selectedPieceFile][selectedPieceRank]->pieceID & 0b11000)) == 0){	//meaing that they are different colors
				legalMoves[legalMovesIndex++] = new Move(selectedPieceFile, selectedPieceRank + i, 0);
				break;
			}
			else{
				break;
			}
		}

		for(int i = 1; selectedPieceRank - i >= 0; i++){
			if(board[selectedPieceFile][selectedPieceRank - i]->pieceID == 0){
				legalMoves[legalMovesIndex++] = new Move(selectedPieceFile, selectedPieceRank - i, 0);
			}
			else if(((board[selectedPieceFile][selectedPieceRank - i]->pieceID & 0b11000) & (board[selectedPieceFile][selectedPieceRank]->pieceID & 0b11000)) == 0){	//meaing that they are different colors
				legalMoves[legalMovesIndex++] = new Move(selectedPieceFile, selectedPieceRank - i, 0);
				break;
			}
			else{
				break;
			}
		}
	}

	legalMoves[legalMovesIndex] = NULL;
}

void knightMoves(Piece ***board, Move **legalMoves, int selectedPieceFile, int selectedPieceRank){
	int legalMovesIndex = 0;
	if(selectedPieceFile - 2 >= 0 && selectedPieceRank - 1 >= 0){
		if((((board[selectedPieceFile][selectedPieceRank]->pieceID & 0b11000) & (board[selectedPieceFile - 2][selectedPieceRank - 1]->pieceID)) != 1)){
			legalMoves[legalMovesIndex++] = new Move(selectedPieceFile - 2, selectedPieceRank - 1, 0);
		}
	}

	if(selectedPieceFile - 1 >= 0 && selectedPieceRank - 2 >= 0){
		if((((board[selectedPieceFile][selectedPieceRank]->pieceID & 0b11000) & (board[selectedPieceFile - 1][selectedPieceRank - 2]->pieceID)) != 1)){
			legalMoves[legalMovesIndex++] = new Move(selectedPieceFile - 1, selectedPieceRank - 2, 0);
		}
	}

	if(selectedPieceFile - 2 >= 0 && selectedPieceRank + 1 < 8){
		if((((board[selectedPieceFile][selectedPieceRank]->pieceID & 0b11000) & (board[selectedPieceFile - 2][selectedPieceRank + 1]->pieceID)) != 1)){
			legalMoves[legalMovesIndex++] = new Move(selectedPieceFile - 2, selectedPieceRank + 1, 0);
		}
	}

	if(selectedPieceFile + 1 < 8 && selectedPieceRank - 2 >= 0){
		if((((board[selectedPieceFile][selectedPieceRank]->pieceID & 0b11000) & (board[selectedPieceFile + 1][selectedPieceRank - 2]->pieceID)) != 1)){
			legalMoves[legalMovesIndex++] = new Move(selectedPieceFile + 1, selectedPieceRank - 2, 0);
		}
	}

	if(selectedPieceFile + 2 < 8 && selectedPieceRank - 1 >= 0){
		if((((board[selectedPieceFile][selectedPieceRank]->pieceID & 0b11000) & (board[selectedPieceFile + 2][selectedPieceRank - 1]->pieceID)) != 1)){
			legalMoves[legalMovesIndex++] = new Move(selectedPieceFile + 2, selectedPieceRank - 1, 0);
		}
	}

	if(selectedPieceFile - 1 >= 0 && selectedPieceRank + 2 < 8){
		if((((board[selectedPieceFile][selectedPieceRank]->pieceID & 0b11000) & (board[selectedPieceFile - 1][selectedPieceRank + 2]->pieceID)) != 1)){
			legalMoves[legalMovesIndex++] = new Move(selectedPieceFile - 1, selectedPieceRank + 2, 0);
		}
	}

	if(selectedPieceFile + 2 < 8 && selectedPieceRank + 1 < 8){
		if((((board[selectedPieceFile][selectedPieceRank]->pieceID & 0b11000) & (board[selectedPieceFile + 2][selectedPieceRank + 1]->pieceID)) != 1)){
			legalMoves[legalMovesIndex++] = new Move(selectedPieceFile + 2, selectedPieceRank + 1, 0);
		}
	}

	if(selectedPieceFile + 1 < 8 && selectedPieceRank + 2 < 8){
		if((((board[selectedPieceFile][selectedPieceRank]->pieceID & 0b11000) & (board[selectedPieceFile + 1][selectedPieceRank + 2]->pieceID)) != 1)){
			legalMoves[legalMovesIndex++] = new Move(selectedPieceFile + 1, selectedPieceRank + 2, 0);
		}
	}

	legalMoves[legalMovesIndex] = NULL;

}
void pawnMoves(Piece ***board, Move **legalMoves, int selectedPieceFile, int selectedPieceRank){
	int legalMovesIndex = 0;

	if((board[selectedPieceFile][selectedPieceRank]->pieceID & BLACK) != 0){	//black pawns
		for(int i = 1; i < 2 + (selectedPieceRank == 1 ? 1 : 0); i++){
			if(board[selectedPieceFile][selectedPieceRank + i]->pieceID == 0){
				if(selectedPieceRank + i == 7){	//promotion
					legalMoves[legalMovesIndex++] = new Move(selectedPieceFile, selectedPieceRank + i, PAWNPROMOQUEENFLAG);	
					legalMoves[legalMovesIndex++] = new Move(selectedPieceFile, selectedPieceRank + i, PAWNPROMOROOKFLAG);	
					legalMoves[legalMovesIndex++] = new Move(selectedPieceFile, selectedPieceRank + i, PAWNPROMOBISHOPFLAG);	
					legalMoves[legalMovesIndex++] = new Move(selectedPieceFile, selectedPieceRank + i, PAWNPROMOKNIGHTFLAG);	
				}
				else{
					legalMoves[legalMovesIndex++] = new Move(selectedPieceFile, selectedPieceRank + i, 0);
				}
			}
			else{
				break;
			}
		}
		if(selectedPieceFile + 1 < 8){
			if((board[selectedPieceFile + 1][selectedPieceRank + 1]->pieceID & WHITE) != 0){
				if(selectedPieceRank + 1 == 7){
					legalMoves[legalMovesIndex++] = new Move(selectedPieceFile + 1, selectedPieceRank + 1, PAWNPROMOQUEENFLAG);	
					legalMoves[legalMovesIndex++] = new Move(selectedPieceFile + 1, selectedPieceRank + 1, PAWNPROMOROOKFLAG);
					legalMoves[legalMovesIndex++] = new Move(selectedPieceFile + 1, selectedPieceRank + 1, PAWNPROMOBISHOPFLAG);	
					legalMoves[legalMovesIndex++] = new Move(selectedPieceFile + 1, selectedPieceRank + 1, PAWNPROMOKNIGHTFLAG);
				}
				else{
					legalMoves[legalMovesIndex++] = new Move(selectedPieceFile + 1, selectedPieceRank + 1, 0);
				}
			}
		
			if(((board[selectedPieceFile + 1][selectedPieceRank]->statusFlag & PAWNENPASFLAG) != 0) && (board[selectedPieceFile + 1][selectedPieceRank]->pieceID & WHITE) != 0){	//EN PASEN
				legalMoves[legalMovesIndex++] = new Move(selectedPieceFile + 1, selectedPieceRank + 1, PAWNENPASFLAG);
			}
		}
		if(selectedPieceFile - 1 >= 0){
			if((board[selectedPieceFile - 1][selectedPieceRank + 1]->pieceID & WHITE) != 0){
				if(selectedPieceRank + 1 == 7){	
					legalMoves[legalMovesIndex++] = new Move(selectedPieceFile - 1, selectedPieceRank + 1, PAWNPROMOQUEENFLAG);	
					legalMoves[legalMovesIndex++] = new Move(selectedPieceFile - 1, selectedPieceRank + 1, PAWNPROMOROOKFLAG);
					legalMoves[legalMovesIndex++] = new Move(selectedPieceFile - 1, selectedPieceRank + 1, PAWNPROMOBISHOPFLAG);	
					legalMoves[legalMovesIndex++] = new Move(selectedPieceFile - 1, selectedPieceRank + 1, PAWNPROMOKNIGHTFLAG);
				}
				else{
					legalMoves[legalMovesIndex++] = new Move(selectedPieceFile - 1, selectedPieceRank + 1, 0);
				}
			}
			
			if(((board[selectedPieceFile - 1][selectedPieceRank]->statusFlag & PAWNENPASFLAG) != 0) && (board[selectedPieceFile - 1][selectedPieceRank]->pieceID & WHITE) != 0){	//EN PASEN
				legalMoves[legalMovesIndex++] = new Move(selectedPieceFile - 1, selectedPieceRank + 1, PAWNENPASFLAG);
			}
		}
	}
	else{	//White Pawns
		for(int i = 1; i < 2 + (selectedPieceRank == 6 ? 1 : 0); i++){
			if(board[selectedPieceFile][selectedPieceRank - i]->pieceID == 0){
				if(selectedPieceRank - i == 0){	//promotion
					legalMoves[legalMovesIndex++] = new Move(selectedPieceFile, selectedPieceRank - i, PAWNPROMOQUEENFLAG);	
					legalMoves[legalMovesIndex++] = new Move(selectedPieceFile, selectedPieceRank - i, PAWNPROMOROOKFLAG);
					legalMoves[legalMovesIndex++] = new Move(selectedPieceFile, selectedPieceRank - i, PAWNPROMOBISHOPFLAG);	
					legalMoves[legalMovesIndex++] = new Move(selectedPieceFile, selectedPieceRank - i, PAWNPROMOKNIGHTFLAG);	
				}
				else{
					legalMoves[legalMovesIndex++] = new Move(selectedPieceFile, selectedPieceRank - i, 0);
				}
			}
			else{
				break;
			}
		}
		if(selectedPieceFile + 1 < 8){
			if((board[selectedPieceFile + 1][selectedPieceRank - 1]->pieceID & BLACK) != 0){
				if(selectedPieceRank - 1 == 0){
					legalMoves[legalMovesIndex++] = new Move(selectedPieceFile + 1, selectedPieceRank - 1, PAWNPROMOQUEENFLAG);	
					legalMoves[legalMovesIndex++] = new Move(selectedPieceFile + 1, selectedPieceRank - 1, PAWNPROMOROOKFLAG);
					legalMoves[legalMovesIndex++] = new Move(selectedPieceFile + 1, selectedPieceRank - 1, PAWNPROMOBISHOPFLAG);	
					legalMoves[legalMovesIndex++] = new Move(selectedPieceFile + 1, selectedPieceRank - 1, PAWNPROMOKNIGHTFLAG);	
				}
				else{
					legalMoves[legalMovesIndex++] = new Move(selectedPieceFile + 1, selectedPieceRank - 1, 0);
				}
			}
			
			if(((board[selectedPieceFile + 1][selectedPieceRank]->statusFlag & PAWNENPASFLAG) != 0) && (board[selectedPieceFile + 1][selectedPieceRank]->pieceID & BLACK) != 0){	//EN PASEN
				legalMoves[legalMovesIndex++] = new Move(selectedPieceFile + 1, selectedPieceRank - 1, PAWNENPASFLAG);
			}
		}

		if(selectedPieceFile - 1 >= 0){
			if((board[selectedPieceFile - 1][selectedPieceRank - 1]->pieceID & BLACK) != 0){
				if(selectedPieceRank - 1 == 0){
					legalMoves[legalMovesIndex++] = new Move(selectedPieceFile - 1, selectedPieceRank - 1, PAWNPROMOQUEENFLAG);	
					legalMoves[legalMovesIndex++] = new Move(selectedPieceFile - 1, selectedPieceRank - 1, PAWNPROMOROOKFLAG);
					legalMoves[legalMovesIndex++] = new Move(selectedPieceFile - 1, selectedPieceRank - 1, PAWNPROMOBISHOPFLAG);	
					legalMoves[legalMovesIndex++] = new Move(selectedPieceFile - 1, selectedPieceRank - 1, PAWNPROMOKNIGHTFLAG);	
				}
				else{
					legalMoves[legalMovesIndex++] = new Move(selectedPieceFile - 1, selectedPieceRank - 1, 0);
				}
			}
			
			if(((board[selectedPieceFile - 1][selectedPieceRank]->statusFlag & PAWNENPASFLAG) != 0) && (board[selectedPieceFile - 1][selectedPieceRank]->pieceID & BLACK) != 0){	//EN PASEN
				legalMoves[legalMovesIndex++] = new Move(selectedPieceFile - 1, selectedPieceRank - 1, PAWNENPASFLAG);
			}
		}
	}

	legalMoves[legalMovesIndex] = NULL;

}

void kingMoves(Piece ***board, Move **legalMoves, int selectedPieceFile, int selectedPieceRank){
	int legalMovesIndex = 0;
	if(selectedPieceFile + 1 < 8 && selectedPieceRank + 1 < 8){
		if(((board[selectedPieceFile + 1][selectedPieceRank + 1]->pieceID & 0b11000) & (board[selectedPieceFile][selectedPieceRank]->pieceID & 0b11000)) == 0){
			legalMoves[legalMovesIndex++] = new Move(selectedPieceFile + 1, selectedPieceRank + 1, 0);
		}
	}
	if(selectedPieceFile + 1 < 8 && selectedPieceRank - 1 >= 0){
		if(((board[selectedPieceFile + 1][selectedPieceRank - 1]->pieceID & 0b11000) & (board[selectedPieceFile][selectedPieceRank]->pieceID & 0b11000)) == 0){
			legalMoves[legalMovesIndex++] = new Move(selectedPieceFile + 1, selectedPieceRank - 1, 0);
		}
	}

	if(selectedPieceFile - 1 >= 0 && selectedPieceRank + 1 < 8){
		if(((board[selectedPieceFile - 1][selectedPieceRank + 1]->pieceID & 0b11000) & (board[selectedPieceFile][selectedPieceRank]->pieceID & 0b11000)) == 0){
			legalMoves[legalMovesIndex++] = new Move(selectedPieceFile - 1, selectedPieceRank + 1, 0);
		}
	}

	if(selectedPieceFile - 1 >= 0 && selectedPieceRank - 1 >= 0){
		if(((board[selectedPieceFile - 1][selectedPieceRank - 1]->pieceID & 0b11000) & (board[selectedPieceFile][selectedPieceRank]->pieceID & 0b11000)) == 0){
			legalMoves[legalMovesIndex++] = new Move(selectedPieceFile - 1, selectedPieceRank - 1, 0);
		}
	}

	if(selectedPieceFile + 1 < 8){
		if(((board[selectedPieceFile + 1][selectedPieceRank]->pieceID & 0b11000) & (board[selectedPieceFile][selectedPieceRank]->pieceID & 0b11000)) == 0){
			legalMoves[legalMovesIndex++] = new Move(selectedPieceFile + 1, selectedPieceRank, 0);
		}
	}

	if(selectedPieceRank + 1 < 8){
		if(((board[selectedPieceFile][selectedPieceRank + 1]->pieceID & 0b11000) & (board[selectedPieceFile][selectedPieceRank]->pieceID & 0b11000)) == 0){
			legalMoves[legalMovesIndex++] = new Move(selectedPieceFile, selectedPieceRank + 1, 0);
		}
	}

	if(selectedPieceFile - 1 >= 0){
		if(((board[selectedPieceFile - 1][selectedPieceRank]->pieceID & 0b11000) & (board[selectedPieceFile][selectedPieceRank]->pieceID & 0b11000)) == 0){
			legalMoves[legalMovesIndex++] = new Move(selectedPieceFile - 1, selectedPieceRank, 0);
		}
	}

	if(selectedPieceRank - 1 >= 0){
		if(((board[selectedPieceFile][selectedPieceRank - 1]->pieceID & 0b11000) & (board[selectedPieceFile][selectedPieceRank]->pieceID & 0b11000)) == 0){
			legalMoves[legalMovesIndex++] = new Move(selectedPieceFile, selectedPieceRank - 1, 0);
		}
	}

	if(selectedPieceFile + 3 < 8){
		if((board[selectedPieceFile][selectedPieceRank]->statusFlag & CANCASTLE != 0) && (board[selectedPieceFile + 3][selectedPieceRank]->statusFlag & CANCASTLE != 0)){
			legalMoves[legalMovesIndex++] = new Move(selectedPieceFile + 2, selectedPieceRank, KINGSIDECASTLE);
		}	
	}

	if(selectedPieceFile - 4 >= 0){
		if(board[selectedPieceFile][selectedPieceRank]->statusFlag & CANCASTLE && board[selectedPieceFile - 4][selectedPieceRank]->statusFlag & CANCASTLE){
			legalMoves[legalMovesIndex++] = new Move(selectedPieceFile - 2, selectedPieceRank, QUEENSIDECASTLE);
		}		
	}

	legalMoves[legalMovesIndex] = NULL;

}

struct Move **Piece::piecesLegalMoves(Piece ***board){
	Move **legalMoves;
	
	uint8_t pieceIDNoColor = pieceID & 0b111;

	if(pieceIDNoColor == 0){
		std::cout << "Not valid Piece" << "\n";
		return NULL;
	}

	int file = rect.x / 100;
	int rank = rect.y / 100;

	switch(pieceIDNoColor){
		case PAWN:{
			legalMoves = new Move*[13];	//12 is the most moves a pawn can make + 1 for NULL this is counting every promotion type as a different move
			pawnMoves(board, legalMoves, file, rank);
			break;
		}

		case KNIGHT:{
			legalMoves = new Move*[9];	//8 is the most moves a knight can make + 1 for NULL
			knightMoves(board, legalMoves, file, rank);
			break;
		}

		case BISHOP:{
			legalMoves = new Move*[14];	//13 is the most moves a bishop can make + 1 for NULL
			slidingMoves(board, legalMoves, file, rank, 1, 0);
			break;
		}

		case ROOK:{
			legalMoves = new Move*[15];	//14 is the most moves a rook can make + 1 for NULL
			slidingMoves(board, legalMoves, file, rank, 0, 1);
			break;
		}

		case QUEEN:{
			legalMoves = new Move*[27];	//26 is the most moves a queen can make + 1 for NULL
			slidingMoves(board, legalMoves, file, rank, 1, 1);
			break;
		}

		case KING:{
			legalMoves = new Move*[9];	//8 is the most moves a queen can make + 1 for NULL
			kingMoves(board, legalMoves, file, rank);
			break;
		}
	}

	return legalMoves;
}

int Piece::move(Piece ***board, Piece *moveSquare, int color, uint8_t pawnPromotionChoice, SDL_Renderer *rend){
	if((pieceID & color) == 0 || pieceID == 0 || (((pieceID & color) & (moveSquare->pieceID & color)) != 0)){
		return 0;	//move not legal because it is not your piece, there is no piece, or you are trying to take your own piece
	}

	Move **legalMoves = piecesLegalMoves(board);

	if(legalMoves == NULL){
		return 0;
	}

	int moveSquareFile = moveSquare->rect.x / 100;
	int moveSquareRank = moveSquare->rect.y / 100;

	int legalMovesIndex = 0;
	while(legalMoves[legalMovesIndex] != NULL){
		if(moveSquareFile == legalMoves[legalMovesIndex]->file && moveSquareRank == legalMoves[legalMovesIndex]->rank){
			for(int i = 0; i < 8; i++){
				for(int j = 0; j < 8; j++){
					board[i][j]->statusFlag &= ~PAWNENPASFLAG;
				}
			}

			if((pieceID & 0b111) == PAWN){
				if(moveSquare->rect.y / 100 - rect.y / 100 == 2 || moveSquare->rect.y / 100 - rect.y / 100 == -2){
					statusFlag |= PAWNENPASFLAG;
				}
				if(legalMoves[legalMovesIndex]->statusFlag & PAWNPROMOQUEENFLAG){
					while(legalMoves[legalMovesIndex]->statusFlag != pawnPromotionChoice){
						legalMovesIndex++;
					}
				}
			}

			if((pieceID & 0b111) == KING){
				statusFlag &= ~CANCASTLE;
			}

			if((pieceID & 0b111) == ROOK){
				statusFlag &= ~CANCASTLE;
			}
			if(legalMoves[legalMovesIndex]->statusFlag == 0){
				//move piece
				moveSquare->pieceID = pieceID;
				//SDL_DestroyTexture(moveSquare->pieceImg);
				moveSquare->pieceImg = pieceImg;
				moveSquare->statusFlag = statusFlag;
				
				//remove piece from where it was
				pieceID = 0;
				pieceImg == NULL;
				statusFlag = 0;


				freeLegalMoves(legalMoves);
				delete[] legalMoves;
				return 1;
			}
			else{
				switch(legalMoves[legalMovesIndex]->statusFlag){
					case PAWNENPASFLAG:{
						//move piece
						moveSquare->pieceID = pieceID;
						SDL_DestroyTexture(moveSquare->pieceImg);
						moveSquare->pieceImg = pieceImg;
						moveSquare->statusFlag = statusFlag;
						
						//remove piece from where it was
						pieceID = 0;
						pieceImg == NULL;
						statusFlag = 0;

						board[moveSquare->rect.x / 100][moveSquare->rect.y / 100 + 1]->pieceID = 0;
						board[moveSquare->rect.x / 100][moveSquare->rect.y / 100 + 1]->pieceImg = NULL;
						board[moveSquare->rect.x / 100][moveSquare->rect.y / 100 + 1]->statusFlag = 0;

						board[moveSquare->rect.x / 100][moveSquare->rect.y / 100 - 1]->pieceID = 0;
						board[moveSquare->rect.x / 100][moveSquare->rect.y / 100 - 1]->pieceImg = NULL;
						board[moveSquare->rect.x / 100][moveSquare->rect.y / 100 - 1]->statusFlag = 0;
					
						break;

					}

					case PAWNPROMOQUEENFLAG:{
						moveSquare->pieceID = QUEEN | color;
						moveSquare->statusFlag = 0;
						if(color == WHITE){
							SDL_DestroyTexture(moveSquare->pieceImg);
							moveSquare->pieceImg = IMG_LoadTexture(rend, "img/whiteQueen.png");
						}
						else{
							SDL_DestroyTexture(moveSquare->pieceImg);
							moveSquare->pieceImg = IMG_LoadTexture(rend, "img/blackQueen.png");
						}

						pieceID = 0;
						SDL_DestroyTexture(pieceImg);
						pieceImg = NULL;
						statusFlag = 0;

						break;
					}

					case PAWNPROMOROOKFLAG:{
						moveSquare->pieceID = ROOK | color;
						moveSquare->statusFlag = 0;
						if(color == WHITE){
							SDL_DestroyTexture(moveSquare->pieceImg);
							moveSquare->pieceImg = IMG_LoadTexture(rend, "img/whiteRook.png");
						}
						else{
							SDL_DestroyTexture(moveSquare->pieceImg);
							moveSquare->pieceImg = IMG_LoadTexture(rend, "img/blackRook.png");
						}

						pieceID = 0;
						SDL_DestroyTexture(pieceImg);
						pieceImg = NULL;
						statusFlag = 0;

						break;
					}

					case PAWNPROMOBISHOPFLAG:{
						moveSquare->pieceID = BISHOP | color;
						moveSquare->statusFlag = 0;
						if(color == WHITE){
							SDL_DestroyTexture(moveSquare->pieceImg);
							moveSquare->pieceImg = IMG_LoadTexture(rend, "img/whiteBishop.png");
						}
						else{
							SDL_DestroyTexture(moveSquare->pieceImg);
							moveSquare->pieceImg = IMG_LoadTexture(rend, "img/blackBishop.png");
						}

						pieceID = 0;
						SDL_DestroyTexture(pieceImg);
						pieceImg = NULL;
						statusFlag = 0;

						break;
					}

					case PAWNPROMOKNIGHTFLAG:{
						moveSquare->pieceID = KNIGHT | color;
						moveSquare->statusFlag = 0;
						if(color == WHITE){
							SDL_DestroyTexture(moveSquare->pieceImg);
							moveSquare->pieceImg = IMG_LoadTexture(rend, "img/whiteKnight.png");
						}
						else{
							SDL_DestroyTexture(moveSquare->pieceImg);
							moveSquare->pieceImg = IMG_LoadTexture(rend, "img/blackKnight.png");
						}

						pieceID = 0;
						SDL_DestroyTexture(pieceImg);
						pieceImg = NULL;
						statusFlag = 0;
						
						break;
					}

					case KINGSIDECASTLE:{
						//move piece
						moveSquare->pieceID = pieceID;
						moveSquare->pieceImg = pieceImg;
						moveSquare->statusFlag = statusFlag;

						//remove piece from where it was
						pieceID = 0;
						pieceImg == NULL;
						statusFlag = 0;
				
						board[rect.x / 100 + 1][rect.y / 100]->pieceID = board[rect.x / 100 + 3][rect.y / 100]->pieceID;
						board[rect.x / 100 + 1][rect.y / 100]->pieceImg = board[rect.x / 100 + 3][rect.y / 100]->pieceImg;
						board[rect.x / 100 + 1][rect.y / 100]->statusFlag = board[rect.x / 100 + 3][rect.y / 100]->statusFlag & ~CANCASTLE;

						board[rect.x / 100 + 3][rect.y / 100]->pieceID = 0;
						board[rect.x / 100 + 3][rect.y / 100]->pieceImg = NULL;
						board[rect.x / 100 + 3][rect.y / 100]->statusFlag = 0;

						break;
					}

					case QUEENSIDECASTLE:{
						//move piece
						moveSquare->pieceID = pieceID;
						moveSquare->pieceImg = pieceImg;
						moveSquare->statusFlag = statusFlag;

						//remove piece from where it was
						pieceID = 0;
						pieceImg == NULL;
						statusFlag = 0;
				
						board[rect.x / 100 - 1][rect.y / 100]->pieceID = board[rect.x / 100 - 4][rect.y / 100]->pieceID;
						board[rect.x / 100 - 1][rect.y / 100]->pieceImg = board[rect.x / 100 - 4][rect.y / 100]->pieceImg;
						board[rect.x / 100 - 1][rect.y / 100]->statusFlag = board[rect.x / 100 - 4][rect.y / 100]->statusFlag & ~CANCASTLE;

						board[rect.x / 100 - 4][rect.y / 100]->pieceID = 0;
						board[rect.x / 100 - 4][rect.y / 100]->pieceImg = NULL;
						board[rect.x / 100 - 4][rect.y / 100]->statusFlag = 0;

						break;
						
					}
				}
				freeLegalMoves(legalMoves);
				delete[] legalMoves;
				return 1;
			}
		}
		legalMovesIndex++;
	}

	return 0;

}

		


