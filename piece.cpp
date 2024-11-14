#include "piece.h"

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

void slidingMoves(Piece ***board, Piece **legalMoves, int selectedPieceFile, int selectedPieceRank, int diagonal, int straight){
	int legalMovesIndex = 0;
	if(diagonal){
		for(int i = 1; (i + selectedPieceFile < 8 && i + selectedPieceRank < 8); i++){
				if(board[selectedPieceFile + i][selectedPieceRank + i]->pieceID == 0){
					legalMoves[legalMovesIndex++] = board[selectedPieceFile + i][selectedPieceRank + i];
				}
				else if(((board[selectedPieceFile + i][selectedPieceRank + i]->pieceID & 0b11000) & (board[selectedPieceFile][selectedPieceRank]->pieceID & 0b11000)) == 0){	//meaing that they are different colors
					legalMoves[legalMovesIndex++] = board[selectedPieceFile + i][selectedPieceRank + i];
					break;
				}
				else{
					break;
				}
		}

		for(int i = 1; (selectedPieceFile - i >= 0 && i + selectedPieceRank < 8); i++){
				if(board[selectedPieceFile - i][selectedPieceRank + i]->pieceID == 0){
					legalMoves[legalMovesIndex++] = board[selectedPieceFile - i][selectedPieceRank + i];
				}
				else if(((board[selectedPieceFile - i][selectedPieceRank + i]->pieceID & 0b11000) & (board[selectedPieceFile][selectedPieceRank]->pieceID & 0b11000)) == 0){	//meaing that they are different colors
					legalMoves[legalMovesIndex++] = board[selectedPieceFile - i][selectedPieceRank + i];
					break;
				}
				else{
					break;
				}
		}

		for(int i = 1; (i + selectedPieceFile < 8 && selectedPieceRank - i >= 0); i++){
				if(board[selectedPieceFile + i][selectedPieceRank - i]->pieceID == 0){
					legalMoves[legalMovesIndex++] = board[selectedPieceFile + i][selectedPieceRank - i];
				}
				else if(((board[selectedPieceFile + i][selectedPieceRank - i]->pieceID & 0b11000) & (board[selectedPieceFile][selectedPieceRank]->pieceID & 0b11000)) == 0){	//meaing that they are different colors
					legalMoves[legalMovesIndex++] = board[selectedPieceFile + i][selectedPieceRank - i];
					break;
				}
				else{
					break;
				}
		}

		for(int i = 1; (selectedPieceFile - i >= 0 && selectedPieceRank - i >= 0); i++){
				if(board[selectedPieceFile - i][selectedPieceRank - i]->pieceID == 0){
					legalMoves[legalMovesIndex++] = board[selectedPieceFile - i][selectedPieceRank - i];
				}
				else if(((board[selectedPieceFile - i][selectedPieceRank - i]->pieceID & 0b11000) & (board[selectedPieceFile][selectedPieceRank]->pieceID & 0b11000)) == 0){	//meaing that they are different colors
					legalMoves[legalMovesIndex++] = board[selectedPieceFile - i][selectedPieceRank - i];
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
				legalMoves[legalMovesIndex++] = board[selectedPieceFile + i][selectedPieceRank];
			}
			else if(((board[selectedPieceFile + i][selectedPieceRank]->pieceID & 0b11000) & (board[selectedPieceFile][selectedPieceRank]->pieceID & 0b11000)) == 0){	//meaing that they are different colors
				legalMoves[legalMovesIndex++] = board[selectedPieceFile + i][selectedPieceRank];
				break;
			}
			else{
				break;
			}
		}

		for(int i = 1; selectedPieceFile - i >= 0; i++){
			if(board[selectedPieceFile - i][selectedPieceRank]->pieceID == 0){
				legalMoves[legalMovesIndex++] = board[selectedPieceFile - i][selectedPieceRank];
			}
			else if(((board[selectedPieceFile - i][selectedPieceRank]->pieceID & 0b11000) & (board[selectedPieceFile][selectedPieceRank]->pieceID & 0b11000)) == 0){	//meaing that they are different colors
				legalMoves[legalMovesIndex++] = board[selectedPieceFile - i][selectedPieceRank];
				break;
			}
			else{
				break;
			}
		}

		for(int i = 1; selectedPieceRank + i < 8; i++){
			if(board[selectedPieceFile][selectedPieceRank + i]->pieceID == 0){
				legalMoves[legalMovesIndex++] = board[selectedPieceFile][selectedPieceRank + i];
			}
			else if(((board[selectedPieceFile][selectedPieceRank + i]->pieceID & 0b11000) & (board[selectedPieceFile][selectedPieceRank]->pieceID & 0b11000)) == 0){	//meaing that they are different colors
				legalMoves[legalMovesIndex++] = board[selectedPieceFile][selectedPieceRank + i];
				break;
			}
			else{
				break;
			}
		}

		for(int i = 1; selectedPieceRank - i >= 0; i++){
			if(board[selectedPieceFile][selectedPieceRank - i]->pieceID == 0){
				legalMoves[legalMovesIndex++] = board[selectedPieceFile][selectedPieceRank - i];
			}
			else if(((board[selectedPieceFile][selectedPieceRank - i]->pieceID & 0b11000) & (board[selectedPieceFile][selectedPieceRank]->pieceID & 0b11000)) == 0){	//meaing that they are different colors
				legalMoves[legalMovesIndex++] = board[selectedPieceFile][selectedPieceRank - i];
				break;
			}
			else{
				break;
			}
		}
	}

	legalMoves[legalMovesIndex] = NULL;
}

void knightMoves(Piece ***board, Piece **legalMoves, int selectedPieceFile, int selectedPieceRank){
	int legalMovesIndex = 0;
	if(selectedPieceFile - 2 >= 0 && selectedPieceRank - 1 >= 0){
		if((((board[selectedPieceFile][selectedPieceRank]->pieceID & 0b11000) & (board[selectedPieceFile - 2][selectedPieceRank - 1]->pieceID)) != 1)){
			legalMoves[legalMovesIndex++] = board[selectedPieceFile - 2][selectedPieceRank - 1];
		}
	}

	if(selectedPieceFile - 1 >= 0 && selectedPieceRank - 2 >= 0){
		if((((board[selectedPieceFile][selectedPieceRank]->pieceID & 0b11000) & (board[selectedPieceFile - 1][selectedPieceRank - 2]->pieceID)) != 1)){
			legalMoves[legalMovesIndex++] = board[selectedPieceFile - 1][selectedPieceRank - 2];
		}
	}

	if(selectedPieceFile - 2 >= 0 && selectedPieceRank + 1 < 8){
		if((((board[selectedPieceFile][selectedPieceRank]->pieceID & 0b11000) & (board[selectedPieceFile - 2][selectedPieceRank + 1]->pieceID)) != 1)){
			legalMoves[legalMovesIndex++] = board[selectedPieceFile - 2][selectedPieceRank + 1];
		}
	}

	if(selectedPieceFile + 1 < 8 && selectedPieceRank - 2 >= 0){
		if((((board[selectedPieceFile][selectedPieceRank]->pieceID & 0b11000) & (board[selectedPieceFile + 1][selectedPieceRank - 2]->pieceID)) != 1)){
			legalMoves[legalMovesIndex++] = board[selectedPieceFile + 1][selectedPieceRank - 2];
		}
	}

	if(selectedPieceFile + 2 < 8 && selectedPieceRank - 1 >= 0){
		if((((board[selectedPieceFile][selectedPieceRank]->pieceID & 0b11000) & (board[selectedPieceFile + 2][selectedPieceRank - 1]->pieceID)) != 1)){
			legalMoves[legalMovesIndex++] = board[selectedPieceFile + 2][selectedPieceRank - 1];
		}
	}

	if(selectedPieceFile - 1 >= 0 && selectedPieceRank + 2 >= 0){
		if((((board[selectedPieceFile][selectedPieceRank]->pieceID & 0b11000) & (board[selectedPieceFile - 1][selectedPieceRank + 2]->pieceID)) != 1)){
			legalMoves[legalMovesIndex++] = board[selectedPieceFile - 1][selectedPieceRank + 2];
		}
	}

	if(selectedPieceFile + 2 < 8 && selectedPieceRank + 1 < 8){
		if((((board[selectedPieceFile][selectedPieceRank]->pieceID & 0b11000) & (board[selectedPieceFile + 2][selectedPieceRank + 1]->pieceID)) != 1)){
			legalMoves[legalMovesIndex++] = board[selectedPieceFile + 2][selectedPieceRank + 1];
		}
	}

	if(selectedPieceFile + 1 < 8 && selectedPieceRank + 2 < 8){
		if((((board[selectedPieceFile][selectedPieceRank]->pieceID & 0b11000) & (board[selectedPieceFile + 1][selectedPieceRank + 2]->pieceID)) != 1)){
			legalMoves[legalMovesIndex++] = board[selectedPieceFile + 1][selectedPieceRank + 2];
		}
	}

	legalMoves[legalMovesIndex] == NULL;

}
void pawnMoves(Piece ***board, Piece **legalMoves, int selectedPieceFile, int selectedPieceRank){
	int legalMovesIndex = 0;

	if((board[selectedPieceFile][selectedPieceRank]->pieceID & BLACK) != 0){
		for(int i = 1; i < 2 + (selectedPieceRank == 1 ? 1 : 0); i++){
			if(board[selectedPieceFile][selectedPieceRank + i]->pieceID == 0){
				legalMoves[legalMovesIndex++] = board[selectedPieceFile][selectedPieceRank + i];
			}
			else{
				break;
			}
		}
		if(selectedPieceFile + 1 < 8){
			if((board[selectedPieceFile + 1][selectedPieceRank + 1]->pieceID & WHITE) != 0){
				legalMoves[legalMovesIndex++] = board[selectedPieceFile + 1][selectedPieceRank + 1];
			}
		
			if(((board[selectedPieceFile + 1][selectedPieceRank]->statusFlag & PAWNENPASFLAG) != 0) && (board[selectedPieceFile + 1][selectedPieceRank]->pieceID & WHITE) != 0){	//EN PASEN
				legalMoves[legalMovesIndex++] = board[selectedPieceFile + 1][selectedPieceRank + 1];
			}
		}
		if(selectedPieceFile - 1 >= 0){
			if((board[selectedPieceFile - 1][selectedPieceRank + 1]->pieceID & WHITE) != 0){
				legalMoves[legalMovesIndex++] = board[selectedPieceFile - 1][selectedPieceRank + 1];
			}
			
			if(((board[selectedPieceFile - 1][selectedPieceRank]->statusFlag & PAWNENPASFLAG) != 0) && (board[selectedPieceFile - 1][selectedPieceRank]->pieceID & WHITE) != 0){	//EN PASEN
				legalMoves[legalMovesIndex++] = board[selectedPieceFile - 1][selectedPieceRank + 1];
			}
		}
	}
	else{
		for(int i = 1; i < 2 + (selectedPieceRank == 6 ? 1 : 0); i++){
			if(board[selectedPieceFile][selectedPieceRank - i]->pieceID == 0){
				legalMoves[legalMovesIndex++] = board[selectedPieceFile][selectedPieceRank - i];
			}
			else{
				break;
			}
		}
		if(selectedPieceFile + 1 < 8){
			if((board[selectedPieceFile + 1][selectedPieceRank - 1]->pieceID & BLACK) != 0){
				legalMoves[legalMovesIndex++] = board[selectedPieceFile + 1][selectedPieceRank - 1];
			}
			
			if(((board[selectedPieceFile + 1][selectedPieceRank]->statusFlag & PAWNENPASFLAG) != 0) && (board[selectedPieceFile + 1][selectedPieceRank]->pieceID & BLACK) != 0){	//EN PASEN
				legalMoves[legalMovesIndex++] = board[selectedPieceFile + 1][selectedPieceRank - 1];
			}
		}

		if(selectedPieceFile - 1 >= 0){
			if((board[selectedPieceFile - 1][selectedPieceRank - 1]->pieceID & BLACK) != 0){
				legalMoves[legalMovesIndex++] = board[selectedPieceFile - 1][selectedPieceRank - 1];
			}
			
			if(((board[selectedPieceFile - 1][selectedPieceRank]->statusFlag & PAWNENPASFLAG) != 0) && (board[selectedPieceFile - 1][selectedPieceRank]->pieceID & BLACK) != 0){	//EN PASEN
				legalMoves[legalMovesIndex++] = board[selectedPieceFile - 1][selectedPieceRank - 1];
			}
		}
	}
	/*if(selectedPieceFile + 1 < 8){
		if((board[selectedPieceFile + 1][selectedPieceRank]->pieceID & PAWN) == 1){
			board[selectedPieceFile + 1][selectedPieceRank]->statusFlag &= ~PAWNENPASFLAG; 
		}
	}
	if(selectedPieceFile - 1 >= 0){
		if((board[selectedPieceFile - 1][selectedPieceRank]->pieceID & PAWN) == 1){
			board[selectedPieceFile - 1][selectedPieceRank]->statusFlag &= ~PAWNENPASFLAG; 
		}
	}*/

	legalMoves[legalMovesIndex] = NULL;

}

void kingMoves(Piece ***board, Piece **legalMoves, int selectedPieceFile, int selectedPieceRank){
	int legalMovesIndex = 0;
	if(selectedPieceFile + 1 < 8 && selectedPieceRank + 1 < 8){
		if(((board[selectedPieceFile + 1][selectedPieceRank + 1]->pieceID & 0b11000) & (board[selectedPieceFile][selectedPieceRank]->pieceID & 0b11000)) == 0){
			legalMoves[legalMovesIndex++] = board[selectedPieceFile + 1][selectedPieceRank + 1];
		}
	}
	if(selectedPieceFile + 1 < 8 && selectedPieceRank - 1 >= 0){
		if(((board[selectedPieceFile + 1][selectedPieceRank - 1]->pieceID & 0b11000) & (board[selectedPieceFile][selectedPieceRank]->pieceID & 0b11000)) == 0){
			legalMoves[legalMovesIndex++] = board[selectedPieceFile + 1][selectedPieceRank - 1];
		}
	}

	if(selectedPieceFile - 1 >= 0 && selectedPieceRank + 1 < 8){
		if(((board[selectedPieceFile - 1][selectedPieceRank + 1]->pieceID & 0b11000) & (board[selectedPieceFile][selectedPieceRank]->pieceID & 0b11000)) == 0){
			legalMoves[legalMovesIndex++] = board[selectedPieceFile - 1][selectedPieceRank + 1];
		}
	}

	if(selectedPieceFile - 1 >= 0 && selectedPieceRank - 1 >= 0){
		if(((board[selectedPieceFile - 1][selectedPieceRank - 1]->pieceID & 0b11000) & (board[selectedPieceFile][selectedPieceRank]->pieceID & 0b11000)) == 0){
			legalMoves[legalMovesIndex++] = board[selectedPieceFile - 1][selectedPieceRank - 1];
		}
	}

	if(selectedPieceFile + 1 < 8){
		if(((board[selectedPieceFile + 1][selectedPieceRank]->pieceID & 0b11000) & (board[selectedPieceFile][selectedPieceRank]->pieceID & 0b11000)) == 0){
			legalMoves[legalMovesIndex++] = board[selectedPieceFile + 1][selectedPieceRank];
		}
	}

	if(selectedPieceRank + 1 < 8){
		if(((board[selectedPieceFile][selectedPieceRank + 1]->pieceID & 0b11000) & (board[selectedPieceFile][selectedPieceRank]->pieceID & 0b11000)) == 0){
			legalMoves[legalMovesIndex++] = board[selectedPieceFile][selectedPieceRank + 1];
		}
	}

	if(selectedPieceFile - 1 >= 0){
		if(((board[selectedPieceFile - 1][selectedPieceRank]->pieceID & 0b11000) & (board[selectedPieceFile][selectedPieceRank]->pieceID & 0b11000)) == 0){
			legalMoves[legalMovesIndex++] = board[selectedPieceFile - 1][selectedPieceRank];
		}
	}

	if(selectedPieceRank - 1 >= 0){
		if(((board[selectedPieceFile][selectedPieceRank - 1]->pieceID & 0b11000) & (board[selectedPieceFile][selectedPieceRank]->pieceID & 0b11000)) == 0){
			legalMoves[legalMovesIndex++] = board[selectedPieceFile][selectedPieceRank - 1];
		}
	}

	legalMoves[legalMovesIndex] = NULL;

}

Piece **Piece::piecesLegalMoves(Piece ***board){
	Piece **legalMoves;
	
	uint8_t pieceIDNoColor = pieceID & 0b111;

	if(pieceIDNoColor == 0){
		std::cout << "Not valid Piece" << "\n";
		return NULL;
	}

	int file = rect.x / 100;
	int rank = rect.y / 100;

	switch(pieceIDNoColor){
		case PAWN:{
			legalMoves = new Piece*[5];	//4 is the most moves a pawn can make + 1 for NULL
			pawnMoves(board, legalMoves, file, rank);
			break;
		}

		case KNIGHT:{
			legalMoves = new Piece*[9];	//8 is the most moves a knight can make + 1 for NULL
			knightMoves(board, legalMoves, file, rank);
			break;
		}

		case BISHOP:{
			legalMoves = new Piece*[14];	//13 is the most moves a bishop can make + 1 for NULL
			slidingMoves(board, legalMoves, file, rank, 1, 0);
			break;
		}

		case ROOK:{
			legalMoves = new Piece*[15];	//14 is the most moves a rook can make + 1 for NULL
			slidingMoves(board, legalMoves, file, rank, 0, 1);
			break;
		}

		case QUEEN:{
			legalMoves = new Piece*[27];	//26 is the most moves a queen can make + 1 for NULL
			slidingMoves(board, legalMoves, file, rank, 1, 1);
			break;
		}

		case KING:{
			legalMoves = new Piece*[9];	//8 is the most moves a queen can make + 1 for NULL
			kingMoves(board, legalMoves, file, rank);
			break;
		}
	}

	return legalMoves;
}

int Piece::move(Piece ***board, Piece *moveSquare, int color){
	if((pieceID & color) == 0 || pieceID == 0 || (((pieceID & color) & (moveSquare->pieceID & color)) != 0)){
		return 0;	//move not legal because it is not your piece, there is no piece, or you are trying to take your own piece
	}

	Piece **legalMoves = piecesLegalMoves(board);

	if(legalMoves == NULL){
		return 0;
	}

	int legalMoveIndex = 0;
	int isMoveLegal = 0;
	while(legalMoves[legalMoveIndex] != NULL){
		if(moveSquare == legalMoves[legalMoveIndex++]){
			isMoveLegal = 1;
			break;
		}
	}

	if(!isMoveLegal){
		return 0;
	}

	for(int i = 0; i < 8; i++){
		for(int j = 0; j < 8; j++){
			board[i][j]->statusFlag &= ~PAWNENPASFLAG;
		}
	}

	if((pieceID & 0b111) == PAWN){
		if(moveSquare->rect.y / 100 - rect.y / 100 == 2 || moveSquare->rect.y / 100 - rect.y / 100 == -2){
			statusFlag |= PAWNENPASFLAG;
		}
		if(moveSquare->rect.x / 100 != rect.x / 100 && moveSquare->pieceID == 0){
			//move piece
			moveSquare->pieceID = pieceID;
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
		
			return 1;
		}
	}

	//move piece
	moveSquare->pieceID = pieceID;
	moveSquare->pieceImg = pieceImg;
	moveSquare->statusFlag = statusFlag;
	
	//remove piece from where it was
	pieceID = 0;
	pieceImg == NULL;
	statusFlag = 0;

	delete[] legalMoves;
	return 1;
}

		


