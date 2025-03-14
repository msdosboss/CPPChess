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

Square::Square(){
	return;
}

Square::Square(int xPos, int yPos){
	piece = NULL;
	rect = {(int) SIZE * xPos, (int) SIZE * yPos, SIZE, SIZE};
}

Piece::Piece(uint8_t pieceIDCon, SDL_Texture *pieceImgCon, uint8_t statusFlagCon, int fileCon, int rankCon){
	pieceID = pieceIDCon;
	pieceImg = pieceImgCon;
	statusFlag = statusFlagCon;
	file = fileCon;
	rank = rankCon;

}

Piece::Piece(int fileCon, int rankCon){
	pieceID = 0;
	pieceImg = NULL;
	statusFlag = 0;
	file = fileCon;
	rank = rankCon;
}

Board::Board(){
	squares = new Square*[8];
	for(int i = 0; i < 8; i++){
		squares[i] = new Square[8];
		for(int j = 0; j < 8; j++){
			squares[i][j] = Square(i, j);
		}
	}
}

void slidingMoves(Board *board, Move **legalMoves, int selectedPieceFile, int selectedPieceRank, int diagonal, int straight){
	int legalMovesIndex = 0;
	if(diagonal){
		for(int i = 1; (i + selectedPieceFile < 8 && i + selectedPieceRank < 8); i++){
				if(board->squares[selectedPieceFile + i][selectedPieceRank + i].piece->pieceID == 0){
					legalMoves[legalMovesIndex++] = new Move(selectedPieceFile + i, selectedPieceRank + i, 0);
				}
				else if(((board->squares[selectedPieceFile + i][selectedPieceRank + i].piece->pieceID & 0b11000) & (board->squares[selectedPieceFile][selectedPieceRank].piece->pieceID & 0b11000)) == 0){	//meaing that they are different colors
					legalMoves[legalMovesIndex++] = new Move(selectedPieceFile + i, selectedPieceRank + i, 0);
					break;
				}
				else{
					break;
				}
		}

		for(int i = 1; (selectedPieceFile - i >= 0 && i + selectedPieceRank < 8); i++){
				if(board->squares[selectedPieceFile - i][selectedPieceRank + i].piece->pieceID == 0){
					legalMoves[legalMovesIndex++] = new Move(selectedPieceFile - i, selectedPieceRank + i, 0);
				}
				else if(((board->squares[selectedPieceFile - i][selectedPieceRank + i].piece->pieceID & 0b11000) & (board->squares[selectedPieceFile][selectedPieceRank].piece->pieceID & 0b11000)) == 0){	//meaing that they are different colors
					legalMoves[legalMovesIndex++] = new Move(selectedPieceFile - i, selectedPieceRank + i, 0);
					break;
				}
				else{
					break;
				}
		}

		for(int i = 1; (i + selectedPieceFile < 8 && selectedPieceRank - i >= 0); i++){
				if(board->squares[selectedPieceFile + i][selectedPieceRank - i].piece->pieceID == 0){
					legalMoves[legalMovesIndex++] = new Move(selectedPieceFile + i, selectedPieceRank - i, 0);
				}
				else if(((board->squares[selectedPieceFile + i][selectedPieceRank - i].piece->pieceID & 0b11000) & (board->squares[selectedPieceFile][selectedPieceRank].piece->pieceID & 0b11000)) == 0){	//meaing that they are different colors
					legalMoves[legalMovesIndex++] = new Move(selectedPieceFile + i, selectedPieceRank - i, 0);
					break;
				}
				else{
					break;
				}
		}

		for(int i = 1; (selectedPieceFile - i >= 0 && selectedPieceRank - i >= 0); i++){
				if(board->squares[selectedPieceFile - i][selectedPieceRank - i].piece->pieceID == 0){
					legalMoves[legalMovesIndex++] = new Move(selectedPieceFile - i, selectedPieceRank - i, 0);
				}
				else if(((board->squares[selectedPieceFile - i][selectedPieceRank - i].piece->pieceID & 0b11000) & (board->squares[selectedPieceFile][selectedPieceRank].piece->pieceID & 0b11000)) == 0){	//meaing that they are different colors
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
			if(board->squares[selectedPieceFile + i][selectedPieceRank].piece->pieceID == 0){
				legalMoves[legalMovesIndex++] = new Move(selectedPieceFile  + i, selectedPieceRank, 0);
			}
			else if(((board->squares[selectedPieceFile + i][selectedPieceRank].piece->pieceID & 0b11000) & (board->squares[selectedPieceFile][selectedPieceRank].piece->pieceID & 0b11000)) == 0){	//meaing that they are different colors
				legalMoves[legalMovesIndex++] = new Move(selectedPieceFile  + i, selectedPieceRank, 0);
				break;
			}
			else{
				break;
			}
		}

		for(int i = 1; selectedPieceFile - i >= 0; i++){
			if(board->squares[selectedPieceFile - i][selectedPieceRank].piece->pieceID == 0){
				legalMoves[legalMovesIndex++] = new Move(selectedPieceFile - i, selectedPieceRank, 0);
			}
			else if(((board->squares[selectedPieceFile - i][selectedPieceRank].piece->pieceID & 0b11000) & (board->squares[selectedPieceFile][selectedPieceRank].piece->pieceID & 0b11000)) == 0){	//meaing that they are different colors
				legalMoves[legalMovesIndex++] = new Move(selectedPieceFile - i, selectedPieceRank, 0);
				break;
			}
			else{
				break;
			}
		}

		for(int i = 1; selectedPieceRank + i < 8; i++){
			if(board->squares[selectedPieceFile][selectedPieceRank + i].piece->pieceID == 0){
				legalMoves[legalMovesIndex++] = new Move(selectedPieceFile, selectedPieceRank + i, 0);
			}
			else if(((board->squares[selectedPieceFile][selectedPieceRank + i].piece->pieceID & 0b11000) & (board->squares[selectedPieceFile][selectedPieceRank].piece->pieceID & 0b11000)) == 0){	//meaing that they are different colors
				legalMoves[legalMovesIndex++] = new Move(selectedPieceFile, selectedPieceRank + i, 0);
				break;
			}
			else{
				break;
			}
		}

		for(int i = 1; selectedPieceRank - i >= 0; i++){
			if(board->squares[selectedPieceFile][selectedPieceRank - i].piece->pieceID == 0){
				legalMoves[legalMovesIndex++] = new Move(selectedPieceFile, selectedPieceRank - i, 0);
			}
			else if(((board->squares[selectedPieceFile][selectedPieceRank - i].piece->pieceID & 0b11000) & (board->squares[selectedPieceFile][selectedPieceRank].piece->pieceID & 0b11000)) == 0){	//meaing that they are different colors
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

void knightMoves(Board *board, Move **legalMoves, int selectedPieceFile, int selectedPieceRank){
	int legalMovesIndex = 0;
	if(selectedPieceFile - 2 >= 0 && selectedPieceRank - 1 >= 0){
		if((((board->squares[selectedPieceFile][selectedPieceRank].piece->pieceID & 0b11000) & (board->squares[selectedPieceFile - 2][selectedPieceRank - 1].piece->pieceID)) != 1)){
			legalMoves[legalMovesIndex++] = new Move(selectedPieceFile - 2, selectedPieceRank - 1, 0);
		}
	}

	if(selectedPieceFile - 1 >= 0 && selectedPieceRank - 2 >= 0){
		if((((board->squares[selectedPieceFile][selectedPieceRank].piece->pieceID & 0b11000) & (board->squares[selectedPieceFile - 1][selectedPieceRank - 2].piece->pieceID)) != 1)){
			legalMoves[legalMovesIndex++] = new Move(selectedPieceFile - 1, selectedPieceRank - 2, 0);
		}
	}

	if(selectedPieceFile - 2 >= 0 && selectedPieceRank + 1 < 8){
		if((((board->squares[selectedPieceFile][selectedPieceRank].piece->pieceID & 0b11000) & (board->squares[selectedPieceFile - 2][selectedPieceRank + 1].piece->pieceID)) != 1)){
			legalMoves[legalMovesIndex++] = new Move(selectedPieceFile - 2, selectedPieceRank + 1, 0);
		}
	}

	if(selectedPieceFile + 1 < 8 && selectedPieceRank - 2 >= 0){
		if((((board->squares[selectedPieceFile][selectedPieceRank].piece->pieceID & 0b11000) & (board->squares[selectedPieceFile + 1][selectedPieceRank - 2].piece->pieceID)) != 1)){
			legalMoves[legalMovesIndex++] = new Move(selectedPieceFile + 1, selectedPieceRank - 2, 0);
		}
	}

	if(selectedPieceFile + 2 < 8 && selectedPieceRank - 1 >= 0){
		if((((board->squares[selectedPieceFile][selectedPieceRank].piece->pieceID & 0b11000) & (board->squares[selectedPieceFile + 2][selectedPieceRank - 1].piece->pieceID)) != 1)){
			legalMoves[legalMovesIndex++] = new Move(selectedPieceFile + 2, selectedPieceRank - 1, 0);
		}
	}

	if(selectedPieceFile - 1 >= 0 && selectedPieceRank + 2 < 8){
		if((((board->squares[selectedPieceFile][selectedPieceRank].piece->pieceID & 0b11000) & (board->squares[selectedPieceFile - 1][selectedPieceRank + 2].piece->pieceID)) != 1)){
			legalMoves[legalMovesIndex++] = new Move(selectedPieceFile - 1, selectedPieceRank + 2, 0);
		}
	}

	if(selectedPieceFile + 2 < 8 && selectedPieceRank + 1 < 8){
		if((((board->squares[selectedPieceFile][selectedPieceRank].piece->pieceID & 0b11000) & (board->squares[selectedPieceFile + 2][selectedPieceRank + 1].piece->pieceID)) != 1)){
			legalMoves[legalMovesIndex++] = new Move(selectedPieceFile + 2, selectedPieceRank + 1, 0);
		}
	}

	if(selectedPieceFile + 1 < 8 && selectedPieceRank + 2 < 8){
		if((((board->squares[selectedPieceFile][selectedPieceRank].piece->pieceID & 0b11000) & (board->squares[selectedPieceFile + 1][selectedPieceRank + 2].piece->pieceID)) != 1)){
			legalMoves[legalMovesIndex++] = new Move(selectedPieceFile + 1, selectedPieceRank + 2, 0);
		}
	}

	legalMoves[legalMovesIndex] = NULL;

}
void pawnMoves(Board *board, Move **legalMoves, int selectedPieceFile, int selectedPieceRank){
	int legalMovesIndex = 0;

	if((board->squares[selectedPieceFile][selectedPieceRank].piece->pieceID & BLACK) != 0){	//black pawns
		for(int i = 1; i < 2 + (selectedPieceRank == 1 ? 1 : 0); i++){
			if(board->squares[selectedPieceFile][selectedPieceRank + i].piece->pieceID == 0){
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
			if((board->squares[selectedPieceFile + 1][selectedPieceRank + 1].piece->pieceID & WHITE) != 0){
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
		
			if(((board->squares[selectedPieceFile + 1][selectedPieceRank].piece->statusFlag & PAWNENPASFLAG) != 0) && (board->squares[selectedPieceFile + 1][selectedPieceRank].piece->pieceID & WHITE) != 0){	//EN PASEN
				legalMoves[legalMovesIndex++] = new Move(selectedPieceFile + 1, selectedPieceRank + 1, PAWNENPASFLAG);
			}
		}
		if(selectedPieceFile - 1 >= 0){
			if((board->squares[selectedPieceFile - 1][selectedPieceRank + 1].piece->pieceID & WHITE) != 0){
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
			
			if(((board->squares[selectedPieceFile - 1][selectedPieceRank].piece->statusFlag & PAWNENPASFLAG) != 0) && (board->squares[selectedPieceFile - 1][selectedPieceRank].piece->pieceID & WHITE) != 0){	//EN PASEN
				legalMoves[legalMovesIndex++] = new Move(selectedPieceFile - 1, selectedPieceRank + 1, PAWNENPASFLAG);
			}
		}
	}
	else{	//White Pawns
		for(int i = 1; i < 2 + (selectedPieceRank == 6 ? 1 : 0); i++){
			if(board->squares[selectedPieceFile][selectedPieceRank - i].piece->pieceID == 0){	//if piece is in front of pawn it can not move forward so break otherwise continue
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
			if((board->squares[selectedPieceFile + 1][selectedPieceRank - 1].piece->pieceID & BLACK) != 0){
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
			
			if(((board->squares[selectedPieceFile + 1][selectedPieceRank].piece->statusFlag & PAWNENPASFLAG) != 0) && (board->squares[selectedPieceFile + 1][selectedPieceRank].piece->pieceID & BLACK) != 0){	//EN PASEN
				legalMoves[legalMovesIndex++] = new Move(selectedPieceFile + 1, selectedPieceRank - 1, PAWNENPASFLAG);
			}
		}

		if(selectedPieceFile - 1 >= 0){
			if((board->squares[selectedPieceFile - 1][selectedPieceRank - 1].piece->pieceID & BLACK) != 0){
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
			
			if(((board->squares[selectedPieceFile - 1][selectedPieceRank].piece->statusFlag & PAWNENPASFLAG) != 0) && (board->squares[selectedPieceFile - 1][selectedPieceRank].piece->pieceID & BLACK) != 0){	//EN PASEN
				legalMoves[legalMovesIndex++] = new Move(selectedPieceFile - 1, selectedPieceRank - 1, PAWNENPASFLAG);
			}
		}
	}

	legalMoves[legalMovesIndex] = NULL;

}

void kingMoves(Board *board, Move **legalMoves, int selectedPieceFile, int selectedPieceRank){
	int legalMovesIndex = 0;
	if(selectedPieceFile + 1 < 8 && selectedPieceRank + 1 < 8){
		if(((board->squares[selectedPieceFile + 1][selectedPieceRank + 1].piece->pieceID & 0b11000) & (board->squares[selectedPieceFile][selectedPieceRank].piece->pieceID & 0b11000)) == 0){
			legalMoves[legalMovesIndex++] = new Move(selectedPieceFile + 1, selectedPieceRank + 1, 0);
		}
	}
	if(selectedPieceFile + 1 < 8 && selectedPieceRank - 1 >= 0){
		if(((board->squares[selectedPieceFile + 1][selectedPieceRank - 1].piece->pieceID & 0b11000) & (board->squares[selectedPieceFile][selectedPieceRank].piece->pieceID & 0b11000)) == 0){
			legalMoves[legalMovesIndex++] = new Move(selectedPieceFile + 1, selectedPieceRank - 1, 0);
		}
	}

	if(selectedPieceFile - 1 >= 0 && selectedPieceRank + 1 < 8){
		if(((board->squares[selectedPieceFile - 1][selectedPieceRank + 1].piece->pieceID & 0b11000) & (board->squares[selectedPieceFile][selectedPieceRank].piece->pieceID & 0b11000)) == 0){
			legalMoves[legalMovesIndex++] = new Move(selectedPieceFile - 1, selectedPieceRank + 1, 0);
		}
	}

	if(selectedPieceFile - 1 >= 0 && selectedPieceRank - 1 >= 0){
		if(((board->squares[selectedPieceFile - 1][selectedPieceRank - 1].piece->pieceID & 0b11000) & (board->squares[selectedPieceFile][selectedPieceRank].piece->pieceID & 0b11000)) == 0){
			legalMoves[legalMovesIndex++] = new Move(selectedPieceFile - 1, selectedPieceRank - 1, 0);
		}
	}

	if(selectedPieceFile + 1 < 8){
		if(((board->squares[selectedPieceFile + 1][selectedPieceRank].piece->pieceID & 0b11000) & (board->squares[selectedPieceFile][selectedPieceRank].piece->pieceID & 0b11000)) == 0){
			legalMoves[legalMovesIndex++] = new Move(selectedPieceFile + 1, selectedPieceRank, 0);
		}
	}

	if(selectedPieceRank + 1 < 8){
		if(((board->squares[selectedPieceFile][selectedPieceRank + 1].piece->pieceID & 0b11000) & (board->squares[selectedPieceFile][selectedPieceRank].piece->pieceID & 0b11000)) == 0){
			legalMoves[legalMovesIndex++] = new Move(selectedPieceFile, selectedPieceRank + 1, 0);
		}
	}

	if(selectedPieceFile - 1 >= 0){
		if(((board->squares[selectedPieceFile - 1][selectedPieceRank].piece->pieceID & 0b11000) & (board->squares[selectedPieceFile][selectedPieceRank].piece->pieceID & 0b11000)) == 0){
			legalMoves[legalMovesIndex++] = new Move(selectedPieceFile - 1, selectedPieceRank, 0);
		}
	}

	if(selectedPieceRank - 1 >= 0){
		if(((board->squares[selectedPieceFile][selectedPieceRank - 1].piece->pieceID & 0b11000) & (board->squares[selectedPieceFile][selectedPieceRank].piece->pieceID & 0b11000)) == 0){
			legalMoves[legalMovesIndex++] = new Move(selectedPieceFile, selectedPieceRank - 1, 0);
		}
	}

	if(selectedPieceFile + 3 < 8){
		if((board->squares[selectedPieceFile][selectedPieceRank].piece->statusFlag & CANCASTLE != 0) && (board->squares[selectedPieceFile + 3][selectedPieceRank].piece->statusFlag & CANCASTLE != 0)){
			legalMoves[legalMovesIndex++] = new Move(selectedPieceFile + 2, selectedPieceRank, KINGSIDECASTLE);
		}	
	}

	if(selectedPieceFile - 4 >= 0){
		if(board->squares[selectedPieceFile][selectedPieceRank].piece->statusFlag & CANCASTLE && board->squares[selectedPieceFile - 4][selectedPieceRank].piece->statusFlag & CANCASTLE){
			legalMoves[legalMovesIndex++] = new Move(selectedPieceFile - 2, selectedPieceRank, QUEENSIDECASTLE);
		}		
	}

	legalMoves[legalMovesIndex] = NULL;

}

/*int Board::isInCheck(uint8_t color, Piece *targetSquare){
	if(color == WHITE){
		int i = -1;
		while(blackPieces[++i] != NULL){
			int j = -1;
			while(blackPieces[i]->attackedSquares[++j] != NULL){
				if(blackPieces[i]->attackedSquares[j] == targetSquare){
					return 1;
				}
			}
		}
	}
	else{
		int i = -1;
		while(blackPieces[++i] != NULL){
			int j = -1;
			while(blackPieces[i]->attackedSquares[++j] != NULL){
				if(blackPieces[i]->attackedSquares[j] == targetSquare){
					return 1;
				}
			}
		}

	}
	return 0;
}*/

Move **Piece::piecesLegalMoves(Board *board){
	Move **legalMoves;
	
	uint8_t pieceIDNoColor = pieceID & 0b111;

	if(pieceIDNoColor == 0){
		std::cout << "Not valid Piece" << "\n";
		return NULL;
	}

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

void Piece::squaresAttacked(Board *board){
	Move **legalMoves = piecesLegalMoves(board);

	int i = 0;
	while(legalMoves[i] != NULL){
		attackedSquares[i] = &board->squares[legalMoves[i]->file][legalMoves[i]->rank];
		i++;
	}
	freeLegalMoves(legalMoves);
	delete [] legalMoves;
}
int Board::move(Square *selectedSquare, Square *moveSquare, int color, uint8_t pawnPromotionChoice, SDL_Renderer *rend){
	if((selectedSquare->piece->pieceID & color) == 0 || selectedSquare->piece->pieceID == 0 || (((selectedSquare->piece->pieceID & color) & (moveSquare->piece->pieceID & color)) != 0)){
		return 0;	//move not legal because it is not your piece, there is no piece, or you are trying to take your own piece
	}

	Move **legalMoves = selectedSquare->piece->piecesLegalMoves(this);

	if(legalMoves == NULL){
		return 0;
	}

	int legalMovesIndex = 0;
	while(legalMoves[legalMovesIndex] != NULL){
		if(moveSquare->piece->file == legalMoves[legalMovesIndex]->file && moveSquare->piece->rank == legalMoves[legalMovesIndex]->rank){
			for(int i = 0; i < 8; i++){
				for(int j = 0; j < 8; j++){
					squares[i][j].piece->statusFlag &= ~PAWNENPASFLAG;
				}
			}

			if((selectedSquare->piece->pieceID & 0b111) == PAWN){
				if(moveSquare->piece->rank - selectedSquare->piece->rank == 2 || moveSquare->piece->rank - selectedSquare->piece->rank == -2){
					selectedSquare->piece->statusFlag |= PAWNENPASFLAG;
				}
				if(legalMoves[legalMovesIndex]->statusFlag & PAWNPROMOQUEENFLAG){
					while(legalMoves[legalMovesIndex]->statusFlag != pawnPromotionChoice){
						legalMovesIndex++;
					}
				}
			}

			if((selectedSquare->piece->pieceID & 0b111) == KING){
				selectedSquare->piece->statusFlag &= ~CANCASTLE;
			}

			if((selectedSquare->piece->pieceID & 0b111) == ROOK){
				selectedSquare->piece->statusFlag &= ~CANCASTLE;
			}

			Piece *inBetweenPiece;	//using inBetweenPiece as a pivot for swapping the 2 pointers
			if(legalMoves[legalMovesIndex]->statusFlag == 0){
				//move piece
				inBetweenPiece = moveSquare->piece;	
				moveSquare->piece = selectedSquare->piece;
				moveSquare->piece->file = moveSquare->rect.x / 100;
				moveSquare->piece->rank = moveSquare->rect.y / 100;
					
				//remove piece from where it was
				selectedSquare->piece = inBetweenPiece;
				selectedSquare->piece->pieceID = 0;
				SDL_DestroyTexture(selectedSquare->piece->pieceImg);
				selectedSquare->piece->pieceImg = NULL;
				selectedSquare->piece->statusFlag = 0;
				selectedSquare->piece->file = selectedSquare->rect.x / 100;
				selectedSquare->piece->rank = selectedSquare->rect.y / 100;

				freeLegalMoves(legalMoves);
				delete[] legalMoves;
				return 1;
			}
			else{
				switch(legalMoves[legalMovesIndex]->statusFlag){
					case PAWNENPASFLAG:{
						//move piece
						inBetweenPiece = moveSquare->piece;
						moveSquare->piece = selectedSquare->piece;
						moveSquare->piece->file = moveSquare->rect.x / 100;
						moveSquare->piece->rank = moveSquare->rect.y / 100;
						
						//remove piece from where it was
						selectedSquare->piece = inBetweenPiece;
						selectedSquare->piece->pieceID = 0;
						SDL_DestroyTexture(selectedSquare->piece->pieceImg);
						selectedSquare->piece->pieceImg = NULL;
						selectedSquare->piece->statusFlag = 0;
						selectedSquare->piece->file = selectedSquare->rect.x / 100;
						selectedSquare->piece->rank = selectedSquare->rect.y / 100;

						squares[moveSquare->rect.x / 100][moveSquare->rect.y / 100 + 1].piece->pieceID = 0;
						squares[moveSquare->rect.x / 100][moveSquare->rect.y / 100 + 1].piece->pieceImg = NULL;
						squares[moveSquare->rect.x / 100][moveSquare->rect.y / 100 + 1].piece->statusFlag = 0;

						squares[moveSquare->rect.x / 100][moveSquare->rect.y / 100 - 1].piece->pieceID = 0;
						squares[moveSquare->rect.x / 100][moveSquare->rect.y / 100 - 1].piece->pieceImg = NULL;
						squares[moveSquare->rect.x / 100][moveSquare->rect.y / 100 - 1].piece->statusFlag = 0;
	
						break;

					}

					case PAWNPROMOQUEENFLAG:{
						moveSquare->piece->pieceID = QUEEN | color;
						moveSquare->piece->statusFlag = 0;
						SDL_DestroyTexture(moveSquare->piece->pieceImg);
						if(color == WHITE){
							moveSquare->piece->pieceImg = IMG_LoadTexture(rend, "img/whiteQueen.png");
						}
						else{
							moveSquare->piece->pieceImg = IMG_LoadTexture(rend, "img/blackQueen.png");
						}

						selectedSquare->piece->pieceID = 0;
						SDL_DestroyTexture(selectedSquare->piece->pieceImg);
						selectedSquare->piece->pieceImg = NULL;
						selectedSquare->piece->statusFlag = 0;

						break;
					}

					case PAWNPROMOROOKFLAG:{
						moveSquare->piece->pieceID = ROOK | color;
						moveSquare->piece->statusFlag = 0;
						SDL_DestroyTexture(moveSquare->piece->pieceImg);
						if(color == WHITE){
							moveSquare->piece->pieceImg = IMG_LoadTexture(rend, "img/whiteRook.png");
						}
						else{
							moveSquare->piece->pieceImg = IMG_LoadTexture(rend, "img/blackRook.png");
						}

						selectedSquare->piece->pieceID = 0;
						SDL_DestroyTexture(selectedSquare->piece->pieceImg);
						selectedSquare->piece->pieceImg = NULL;
						selectedSquare->piece->statusFlag = 0;

						break;
					}

					case PAWNPROMOBISHOPFLAG:{
						moveSquare->piece->pieceID = BISHOP | color;
						moveSquare->piece->statusFlag = 0;
						SDL_DestroyTexture(moveSquare->piece->pieceImg);
						if(color == WHITE){
							moveSquare->piece->pieceImg = IMG_LoadTexture(rend, "img/whiteBishop.png");
						}
						else{
							moveSquare->piece->pieceImg = IMG_LoadTexture(rend, "img/blackBishop.png");
						}

						selectedSquare->piece->pieceID = 0;
						SDL_DestroyTexture(selectedSquare->piece->pieceImg);
						selectedSquare->piece->pieceImg = NULL;
						selectedSquare->piece->statusFlag = 0;

						break;

					}

					case PAWNPROMOKNIGHTFLAG:{
						moveSquare->piece->pieceID = KNIGHT | color;
						moveSquare->piece->statusFlag = 0;
						SDL_DestroyTexture(moveSquare->piece->pieceImg);
						if(color == WHITE){
							moveSquare->piece->pieceImg = IMG_LoadTexture(rend, "img/whiteKnight.png");
						}
						else{
							moveSquare->piece->pieceImg = IMG_LoadTexture(rend, "img/blackKnight.png");
						}

						selectedSquare->piece->pieceID = 0;
						SDL_DestroyTexture(selectedSquare->piece->pieceImg);
						selectedSquare->piece->pieceImg = NULL;
						selectedSquare->piece->statusFlag = 0;

						break;


					}

					case KINGSIDECASTLE:{
						//move King
						inBetweenPiece = moveSquare->piece;	
						moveSquare->piece = selectedSquare->piece;
						moveSquare->piece->file = moveSquare->rect.x / 100;
						moveSquare->piece->rank = moveSquare->rect.y / 100;
							
						//remove King from where it was
						selectedSquare->piece = inBetweenPiece;
						selectedSquare->piece->pieceID = 0;
						SDL_DestroyTexture(selectedSquare->piece->pieceImg);
						selectedSquare->piece->pieceImg = NULL;
						selectedSquare->piece->statusFlag = 0;
						selectedSquare->piece->file = selectedSquare->rect.x / 100;
						selectedSquare->piece->rank = selectedSquare->rect.y / 100;

						//move Rook
						inBetweenPiece = squares[selectedSquare->rect.x / 100 + 1][selectedSquare->rect.y / 100].piece;
						squares[selectedSquare->rect.x / 100 + 1][selectedSquare->rect.y / 100].piece = squares[selectedSquare->rect.x / 100 + 3][selectedSquare->rect.y / 100].piece;
						squares[selectedSquare->rect.x / 100 + 1][selectedSquare->rect.y / 100].piece->file = selectedSquare->rect.x / 100 + 1;
						squares[selectedSquare->rect.x / 100 + 1][selectedSquare->rect.y / 100].piece->rank = selectedSquare->rect.y / 100;

						//remove Rook from where it was
						squares[selectedSquare->rect.x / 100 + 3][selectedSquare->rect.y / 100].piece = inBetweenPiece;
						squares[selectedSquare->rect.x / 100 + 3][selectedSquare->rect.y / 100].piece->pieceID = 0;
						SDL_DestroyTexture(squares[selectedSquare->rect.x / 100 + 3][selectedSquare->rect.y / 100].piece->pieceImg);
						squares[selectedSquare->rect.x / 100 + 3][selectedSquare->rect.y / 100].piece->pieceImg = NULL;
						squares[selectedSquare->rect.x / 100 + 3][selectedSquare->rect.y / 100].piece->statusFlag = 0;
						squares[selectedSquare->rect.x / 100 + 3][selectedSquare->rect.y / 100].piece->file = selectedSquare->rect.x / 100 + 3;
						squares[selectedSquare->rect.x / 100 + 3][selectedSquare->rect.y / 100].piece->rank = selectedSquare->rect.y / 100;

						break;
					}

					case QUEENSIDECASTLE:{
						//move King
						inBetweenPiece = moveSquare->piece;	
						moveSquare->piece = selectedSquare->piece;
						moveSquare->piece->file = moveSquare->rect.x / 100;
						moveSquare->piece->rank = moveSquare->rect.y / 100;
							
						//remove King from where it was
						selectedSquare->piece = inBetweenPiece;
						selectedSquare->piece->pieceID = 0;
						SDL_DestroyTexture(selectedSquare->piece->pieceImg);
						selectedSquare->piece->pieceImg = NULL;
						selectedSquare->piece->statusFlag = 0;
						selectedSquare->piece->file = selectedSquare->rect.x / 100;
						selectedSquare->piece->rank = selectedSquare->rect.y / 100;

						//move Rook
						inBetweenPiece = squares[selectedSquare->rect.x / 100 - 1][selectedSquare->rect.y / 100].piece;
						squares[selectedSquare->rect.x / 100 - 1][selectedSquare->rect.y / 100].piece = squares[selectedSquare->rect.x / 100 - 4][selectedSquare->rect.y / 100].piece;
						squares[selectedSquare->rect.x / 100 - 1][selectedSquare->rect.y / 100].piece->file = selectedSquare->rect.x / 100 - 1;
						squares[selectedSquare->rect.x / 100 - 1][selectedSquare->rect.y / 100].piece->rank = selectedSquare->rect.y / 100;

						//remove Rook from where it was
						squares[selectedSquare->rect.x / 100 - 4][selectedSquare->rect.y / 100].piece = inBetweenPiece;
						squares[selectedSquare->rect.x / 100 - 4][selectedSquare->rect.y / 100].piece->pieceID = 0;
						SDL_DestroyTexture(squares[selectedSquare->rect.x / 100 - 4][selectedSquare->rect.y / 100].piece->pieceImg);
						squares[selectedSquare->rect.x / 100 - 4][selectedSquare->rect.y / 100].piece->pieceImg = NULL;
						squares[selectedSquare->rect.x / 100 - 4][selectedSquare->rect.y / 100].piece->statusFlag = 0;
						squares[selectedSquare->rect.x / 100 - 4][selectedSquare->rect.y / 100].piece->file = selectedSquare->rect.x / 100 - 4;
						squares[selectedSquare->rect.x / 100 - 4][selectedSquare->rect.y / 100].piece->rank = selectedSquare->rect.y / 100;

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

		


