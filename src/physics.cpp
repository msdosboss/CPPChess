#include "physics.hpp"
#include "openBook.hpp"

Bitboard knightAttacks[64];
Bitboard kingAttacks[64];
// Masks to prevent wrapping around the edges
const uint64_t fileA = 0x0101010101010101ULL;
const uint64_t fileB = 0x0202020202020202ULL;
const uint64_t fileG = 0x4040404040404040ULL;
const uint64_t fileH = 0x8080808080808080ULL;
const uint64_t RANK_2 = 0x000000000000FF00ULL;
const uint64_t RANK_7 = 0x00FF000000000000ULL;

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
        
        attacks |= (b << 7) & ~fileH; // Up-Left
        attacks |= (b << 8);          // Up
        attacks |= (b << 9) & ~fileA; // Up-Right

        attacks |= (b >> 1) & ~fileH; // Left
        attacks |= (b << 1) & ~fileA; // Right

        attacks |= (b >> 9) & ~fileH; // Down-Left
        attacks |= (b >> 8);          // Down
        attacks |= (b >> 7) & ~fileA; // Down-Right

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
        if(((b << 8) & ~boardState.occupiedSquares[2]) && (b & RANK_2) != 0){
            squaresAttacked |= (b << 16) & ~(boardState.occupiedSquares[2]);
        }
        //capture
        squaresAttacked |= ((b << 9) & ~fileA) & (boardState.occupiedSquares[BLACK] | (1ULL << boardState.enPassantSquare));
        squaresAttacked |= ((b << 7) & ~fileH) & (boardState.occupiedSquares[BLACK] | (1ULL << boardState.enPassantSquare));
    }
    else{
        //Moving forward
        squaresAttacked |= (b >> 8) & ~boardState.occupiedSquares[2]; 
        if(((b >> 8) & ~boardState.occupiedSquares[2]) && (b & RANK_7) != 0){
            squaresAttacked |= (b >> 16) & ~(boardState.occupiedSquares[2]);
        }
        //capture
        squaresAttacked |= ((b >> 9) & ~fileH) & (boardState.occupiedSquares[WHITE] | (1ULL << boardState.enPassantSquare));
        squaresAttacked |= ((b >> 7) & ~fileA) & (boardState.occupiedSquares[WHITE] | (1ULL << boardState.enPassantSquare));
    }
    return squaresAttacked;
}


uint64_t generateStraightMoves(BoardState& boardState, int square, int color){
    uint64_t b = 1ULL << square;
    uint64_t squaresAttacked = 0ULL;

    //Up
    uint64_t current = b;
    while(true){
        current = (current << 8) & ~boardState.occupiedSquares[color];
        if(current == 0){
            break;
        }
        squaresAttacked |= current;
        //collides into opponents pieces
        if(current & boardState.occupiedSquares[2]){
            break;
        }
    }
    current = b;
    //Down
    while(true){
        current = (current >> 8) & ~boardState.occupiedSquares[color];
        if(current == 0){
            break;
        }
        squaresAttacked |= current;
        //collides into opponents pieces
        if(current & boardState.occupiedSquares[2]){
            break;
        }
    }
    current = b;
    //Right
    while(true){
        current = (current << 1) & ~(boardState.occupiedSquares[color] | fileA);
        if(current == 0){
            break;
        }
        squaresAttacked |= current;
        //collides into opponents pieces
        if(current & boardState.occupiedSquares[2]){
            break;
        }
    }
    current = b;
    //Left
    while(true){
        current = (current >> 1) & ~(boardState.occupiedSquares[color] | fileH);
        if(current == 0){
            break;
        }
        squaresAttacked |= current;
        //collides into opponents pieces
        if(current & boardState.occupiedSquares[2]){
            break;
        }
    }

    return squaresAttacked;
}

uint64_t generateDiagnalMoves(BoardState& boardState, int square, int color){
    uint64_t b = 1ULL << square;
    uint64_t squaresAttacked = 0ULL;

    uint64_t current = b;
    //Up and left
    while(true){
        current = (current << 7) & ~(boardState.occupiedSquares[color] | fileH);
        if(current == 0){
            break;
        }
        squaresAttacked |= current;
        //collides into opponents pieces
        if(current & boardState.occupiedSquares[2]){
            break;
        }
    }
    current = b;
    //Up and right
    while(true){
        current = (current << 9) & ~(boardState.occupiedSquares[color] | fileA);
        if(current == 0){
            break;
        }
        squaresAttacked |= current;
        //collides into opponents pieces
        if(current & boardState.occupiedSquares[2]){
            break;
        }
    }
    current = b;
    //Down and right
    while(true){
        current = (current >> 7) & ~(boardState.occupiedSquares[color] | fileA);
        if(current == 0){
            break;
        }
        squaresAttacked |= current;
        //collides into opponents pieces
        if(current & boardState.occupiedSquares[2]){
            break;
        }
    }
    current = b;
    //Down and left
    while(true){
        current = (current >> 9) & ~(boardState.occupiedSquares[color] | fileH);
        if(current == 0){
            break;
        }
        squaresAttacked |= current;
        //collides into opponents pieces
        if(current & boardState.occupiedSquares[2]){
            break;
        }
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

MoveList generateMoves(BoardState& boardState, int color){
    MoveList moveList;
    moveList.count = 0;

    for(int pieceType = PAWN; pieceType <= KING; pieceType++){
        Bitboard pieceBoard = boardState.pieces[color][pieceType]; 
        while(pieceBoard != 0){
            //Find index of piece
            int sourceSquare = __builtin_ctzll(pieceBoard);
            if(pieceType != PAWN){
                Bitboard attackBoard;
                switch(pieceType){
                    case BISHOP:
                        attackBoard = generateBishopMoves(boardState, sourceSquare, color);
                        break;
                    case KNIGHT:
                        attackBoard = generateKnightMoves(boardState, sourceSquare, color);
                        break;
                    case ROOK:
                        attackBoard = generateRookMoves(boardState, sourceSquare, color);
                        break;
                    case QUEEN:
                        attackBoard = generateQueenMoves(boardState, sourceSquare, color);
                        break;
                    case KING:
                        attackBoard = generateKingMoves(boardState, sourceSquare, color);
                        int oppColor = (color == WHITE) ? BLACK : WHITE;

                        if (color == WHITE && sourceSquare == 4) { // 4 is e1
                            // White Kingside (O-O)
                            if (boardState.castlingRights.kingSideCastleWhite) {
                                // Check occupancy: f1 (5) and g1 (6) must be empty
                                if (!isOccupied(boardState.occupiedSquares[2], 5) && 
                                    !isOccupied(boardState.occupiedSquares[2], 6)) {
                                    
                                    // Cannot castle out of check (e1) or through check (f1)
                                    // The final square (g1) is naturally checked by your generateLegalMoves filter later!
                                    if (!isSquareAttacked(boardState, 4, oppColor) && 
                                        !isSquareAttacked(boardState, 5, oppColor)) {
                                        
                                        moveList.moves[moveList.count].raw = 0;
                                        moveList.moves[moveList.count].source = 4;
                                        moveList.moves[moveList.count].dest = 6;
                                        moveList.moves[moveList.count].flags = KINGCASTLE;
                                        moveList.count++;
                                    }
                                }
                            }
                            // White Queenside (O-O-O)
                            if (boardState.castlingRights.queenSideCastleWhite) {
                                // Check occupancy: d1 (3), c1 (2), and b1 (1) must be empty
                                if (!isOccupied(boardState.occupiedSquares[2], 3) && 
                                    !isOccupied(boardState.occupiedSquares[2], 2) && 
                                    !isOccupied(boardState.occupiedSquares[2], 1)) {
                                    
                                    // Cannot castle out of check (e1) or through check (d1)
                                    if (!isSquareAttacked(boardState, 4, oppColor) && 
                                        !isSquareAttacked(boardState, 3, oppColor)) {
                                        
                                        moveList.moves[moveList.count].raw = 0;
                                        moveList.moves[moveList.count].source = 4;
                                        moveList.moves[moveList.count].dest = 2;
                                        moveList.moves[moveList.count].flags = QUEENCASTLE;
                                        moveList.count++;
                                    }
                                }
                            }
                        } 
                        else if (color == BLACK && sourceSquare == 60) { // 60 is e8
                            // Black Kingside (O-O)
                            if (boardState.castlingRights.kingSideCastleBlack) {
                                // Check occupancy: f8 (61) and g8 (62) must be empty
                                if (!isOccupied(boardState.occupiedSquares[2], 61) && 
                                    !isOccupied(boardState.occupiedSquares[2], 62)) {
                                    
                                    if (!isSquareAttacked(boardState, 60, oppColor) && 
                                        !isSquareAttacked(boardState, 61, oppColor)) {
                                        
                                        moveList.moves[moveList.count].raw = 0;
                                        moveList.moves[moveList.count].source = 60;
                                        moveList.moves[moveList.count].dest = 62;
                                        moveList.moves[moveList.count].flags = KINGCASTLE;
                                        moveList.count++;
                                    }
                                }
                            }
                            // Black Queenside (O-O-O)
                            if (boardState.castlingRights.queenSideCastleBlack) {
                                // Check occupancy: d8 (59), c8 (58), b8 (57) must be empty
                                if (!isOccupied(boardState.occupiedSquares[2], 59) && 
                                    !isOccupied(boardState.occupiedSquares[2], 58) && 
                                    !isOccupied(boardState.occupiedSquares[2], 57)) {
                                    
                                    if (!isSquareAttacked(boardState, 60, oppColor) && 
                                        !isSquareAttacked(boardState, 59, oppColor)) {
                                        
                                        moveList.moves[moveList.count].raw = 0;
                                        moveList.moves[moveList.count].source = 60;
                                        moveList.moves[moveList.count].dest = 58;
                                        moveList.moves[moveList.count].flags = QUEENCASTLE;
                                        moveList.count++;
                                    }
                                }
                            }
                        }
                            break;
                }
                while(attackBoard != 0){
                    int destSquare = __builtin_ctzll(attackBoard);

                    moveList.moves[moveList.count].raw = 0;
                    moveList.moves[moveList.count].source = sourceSquare;
                    moveList.moves[moveList.count].dest = destSquare;
                    //Im using the entire board because I trust that generate
                    //moves will not give me moves to allie squares
                    if(isOccupied(boardState.occupiedSquares[2], destSquare)){
                        moveList.moves[moveList.count].flags = CAPTUREMOVE;
                    }

                    moveList.count++;
                    clearBit(attackBoard, destSquare);
                }
                
            }
            //Seperating the pawn since it has a lot of special moves
            else{
                Bitboard attackBoard = generatePawnMoves(boardState, sourceSquare, color);
                while(attackBoard != 0){
                    int destSquare = __builtin_ctzll(attackBoard);

                    moveList.moves[moveList.count].raw = 0;
                    moveList.moves[moveList.count].source = sourceSquare;
                    moveList.moves[moveList.count].dest = destSquare;
                    //Double Move detection
                    if(abs(sourceSquare - destSquare) == 16){
                        moveList.moves[moveList.count].flags |= DOUBLEMOVE;
                    
                    }
                    //Im using the entire board because I trust that generate
                    //moves will not give me moves to allies squares
                    else if(isOccupied(boardState.occupiedSquares[2], destSquare)){
                        moveList.moves[moveList.count].flags |= CAPTUREMOVE;
                    }
                    //If not double Move, Normal Capture or single move it has to be a en passant
                    else if(destSquare == boardState.enPassantSquare){
                        moveList.moves[moveList.count].flags |= ENPASSANTCAPTURE;
                    
                    }
                    //Pawn Promotion
                    if(destSquare < 8 || destSquare > 55){
                        moveList.moves[moveList.count].flags |= QUEENPROMO;
                        uint8_t isCapture = moveList.moves[moveList.count].flags & CAPTUREMOVE;
                        moveList.count++;
                        moveList.moves[moveList.count] = moveList.moves[moveList.count - 1];
                        moveList.moves[moveList.count].flags = isCapture | ROOKPROMO;
                        moveList.count++;
                        moveList.moves[moveList.count] = moveList.moves[moveList.count - 1];
                        moveList.moves[moveList.count].flags = isCapture | BISHOPPROMO;
                        moveList.count++;
                        moveList.moves[moveList.count] = moveList.moves[moveList.count - 1];
                        moveList.moves[moveList.count].flags = isCapture | KNIGHTPROMO;
                        
                    }

                    moveList.count++;
                    clearBit(attackBoard, destSquare);
                
                }
            
            }

            //Clear of temp bitboard to find next piece
            clearBit(pieceBoard, sourceSquare);
        }
    }

    return moveList;
}

bool isSquareAttacked(BoardState& boardState, int square, int attackerColor) {
    int defenderColor = (attackerColor == WHITE) ? BLACK : WHITE;
    uint64_t b = 1ULL << square;

    if ((knightAttacks[square] & boardState.pieces[attackerColor][KNIGHT]) != 0) return true;
    if ((kingAttacks[square]   & boardState.pieces[attackerColor][KING])   != 0) return true;

    // Pawns attack diagonally forward
    if (attackerColor == WHITE) {
        // White pawns attack "up" the board
        if (((b >> 9) & ~fileH) & boardState.pieces[WHITE][PAWN]) return true;
        if (((b >> 7) & ~fileA) & boardState.pieces[WHITE][PAWN]) return true;
    } else {
        // Black pawns attack "down" the board
        if (((b << 9) & ~fileA) & boardState.pieces[BLACK][PAWN]) return true;
        if (((b << 7) & ~fileH) & boardState.pieces[BLACK][PAWN]) return true;
    }

    //Generate a move from the target square and see if it collides with an enemy slider.
    if ((generateBishopMoves(boardState, square, defenderColor) &
        (boardState.pieces[attackerColor][BISHOP] | boardState.pieces[attackerColor][QUEEN])) != 0) return true;

    if ((generateRookMoves(boardState, square, defenderColor) &
        (boardState.pieces[attackerColor][ROOK] | boardState.pieces[attackerColor][QUEEN])) != 0) return true;

    return false;
}

MoveList generateLegalMoves(BoardState boardState){
    MoveList pseudoMoves = generateMoves(boardState, boardState.sideToMove);
    MoveList legalMoves;
    legalMoves.count = 0;

    int myColor = boardState.sideToMove;
    int oppColor = (myColor == WHITE) ? BLACK : WHITE;

    for (int i = 0; i < pseudoMoves.count; i++) {
        Move move = pseudoMoves.moves[i];
        UndoState undo;

        //Make the pseudo-legal move
        makeMove(boardState, move, undo);

        //Find where our king is NOW
        int myKingSquare = __builtin_ctzll(boardState.pieces[myColor][KING]);

        //If the king is NOT attacked, the move is legal!
        if (!isSquareAttacked(boardState, myKingSquare, oppColor)) {
            legalMoves.moves[legalMoves.count] = move;
            legalMoves.count++;
        }

        //ALWAYS unmake the move to restore the board for the next iteration
        unmakeMove(boardState, move, undo);
    }

    return legalMoves;
}

void unmakeMove(BoardState& boardState, Move move, UndoState undoState){
    int destPieceType = boardState.pieceArray[move.dest];
    //This should be the opposite of because they would have just moved 
    int color = (boardState.sideToMove == WHITE) ? BLACK : WHITE;
    /*for(int i = PAWN; i <= KING; i++){
        if(isOccupied(boardState.pieces[color][i], move.dest)){
            destPieceType = i;
        }
    }*/

    clearBit(boardState.pieces[color][destPieceType], move.dest);
    boardState.pieceArray[move.dest] = -1;
    switch(move.flags){
        case QUIETMOVE:
        case DOUBLEMOVE:
            boardState.pieceArray[move.source] = destPieceType;
            setBit(boardState.pieces[color][destPieceType], move.source);
            break; 

        case KINGCASTLE:
            boardState.pieceArray[move.source] = destPieceType;
            setBit(boardState.pieces[color][destPieceType], move.source);
            boardState.pieceArray[move.source + 1] = -1;
            boardState.pieceArray[move.source + 3] = ROOK;
            clearBit(boardState.pieces[color][ROOK], move.source + 1);
            setBit(boardState.pieces[color][ROOK], move.source + 3);
            break; 

        case QUEENCASTLE:
            boardState.pieceArray[move.source] = destPieceType;
            setBit(boardState.pieces[color][destPieceType], move.source);
            boardState.pieceArray[move.source - 1] = -1;
            boardState.pieceArray[move.source - 4] = ROOK;
            clearBit(boardState.pieces[color][ROOK], move.source - 1);
            setBit(boardState.pieces[color][ROOK], move.source - 4);
            break; 

        case CAPTUREMOVE:
            boardState.pieceArray[move.source] = destPieceType;
            setBit(boardState.pieces[color][destPieceType], move.source);
            //Restore the captured piece
            boardState.pieceArray[move.dest] = undoState.capturedPieceType;
            setBit(boardState.pieces[boardState.sideToMove][undoState.capturedPieceType], move.dest);
            break; 

        case ENPASSANTCAPTURE:
            boardState.pieceArray[move.source] = destPieceType;
            setBit(boardState.pieces[color][destPieceType], move.source);
            if(color == WHITE){
                boardState.pieceArray[move.dest - 8] = PAWN;
                setBit(boardState.pieces[boardState.sideToMove][PAWN], move.dest - 8);
            }
            else{
                boardState.pieceArray[move.dest + 8] = PAWN;
                setBit(boardState.pieces[boardState.sideToMove][PAWN], move.dest + 8);
            }
            break; 

        case KNIGHTPROMO:
        case BISHOPPROMO:
        case ROOKPROMO:
        case QUEENPROMO:
            boardState.pieceArray[move.source] = PAWN;
            setBit(boardState.pieces[color][PAWN], move.source);
            break; 

        case KNIGHTPROMOCAPTURE:
        case BISHOPPROMOCAPTURE:
        case ROOKPROMOCAPTURE:
        case QUEENPROMOCAPTURE:
            boardState.pieceArray[move.source] = PAWN;
            setBit(boardState.pieces[color][PAWN], move.source);
            //Restore the captured piece
            boardState.pieceArray[move.dest] = undoState.capturedPieceType;
            setBit(boardState.pieces[boardState.sideToMove][undoState.capturedPieceType], move.dest);
            break; 
    }

    boardState.sideToMove = color;
    boardState.enPassantSquare = undoState.enPassantSquare;
    boardState.castlingRights = undoState.castling;
    populateOccupiedSquares(boardState);
    boardState.zobristHash = undoState.oldZobristHash;
}

void makeMove(BoardState& boardState, Move move, UndoState& undoState){
    int pieceType = boardState.pieceArray[move.source];
    int destPieceType = boardState.pieceArray[move.dest];
    int opposingColor = (boardState.sideToMove == WHITE) ? BLACK : WHITE;
    /*for(int i = PAWN; i <= KING; i++){
        if(isOccupied(boardState.pieces[boardState.sideToMove][i], move.source)){
            pieceType = i;
        }
        if(isOccupied(boardState.pieces[opposingColor][i], move.dest)){
            destPieceType = i;
        }
    }*/
    undoState.oldZobristHash = boardState.zobristHash;
    undoState.capturedPieceType = destPieceType;
    undoState.enPassantSquare = boardState.enPassantSquare;
    undoState.castling = boardState.castlingRights;

    int zobristPieceIndex = pieceType + (6 * boardState.sideToMove);
    boardState.zobristHash ^= zobristSideToMove;
    //Removing piece from source square
    boardState.zobristHash ^= zobristTable[zobristPieceIndex][move.source];

    //Removing all the states from the hash inorder to add back at end
    zobristFlags(boardState);

    if(pieceType == -1){
        std::cerr << "Failed to find piece in makeMove on square " << move.source << std::endl;
        return;
    }
    if (pieceType == KING) {
        if (boardState.sideToMove == WHITE) {
            boardState.castlingRights.kingSideCastleWhite = 0;
            boardState.castlingRights.queenSideCastleWhite = 0;
        } else {
            boardState.castlingRights.kingSideCastleBlack = 0;
            boardState.castlingRights.queenSideCastleBlack = 0;
        }
    } else if (pieceType == ROOK) {
        // If a rook moves from its starting square, remove that specific castling right
        if (move.source == 0){ 
            boardState.castlingRights.queenSideCastleWhite = 0;
        }
        else if(move.source == 7){
            boardState.castlingRights.kingSideCastleWhite = 0;
        }
        else if(move.source == 56){
            boardState.castlingRights.queenSideCastleBlack = 0; 
        }
        else if(move.source == 63){
            boardState.castlingRights.kingSideCastleBlack = 0;
        }
    }
    if (destPieceType == ROOK) {
        if (move.dest == 0){ 
            boardState.castlingRights.queenSideCastleWhite = 0;
        }
        else if (move.dest == 7){
            boardState.castlingRights.kingSideCastleWhite = 0;
        }
        else if (move.dest == 56){
            boardState.castlingRights.queenSideCastleBlack = 0;
        }
        else if (move.dest == 63){
            boardState.castlingRights.kingSideCastleBlack = 0;
        }
    }
    boardState.pieceArray[move.source] = -1;
    clearBit(boardState.pieces[boardState.sideToMove][pieceType], move.source);
    boardState.enPassantSquare = -1;
    switch(move.flags){
        case QUIETMOVE:
            //Adding piece to dest square
            boardState.zobristHash ^= zobristTable[zobristPieceIndex][move.dest];
            setBit(boardState.pieces[boardState.sideToMove][pieceType], move.dest); 
            boardState.pieceArray[move.dest] = pieceType;
            break;
            
        case DOUBLEMOVE:
            //Adding piece to dest square
            boardState.zobristHash ^= zobristTable[zobristPieceIndex][move.dest];
            setBit(boardState.pieces[boardState.sideToMove][pieceType], move.dest); 
            boardState.pieceArray[move.dest] = pieceType;
            if(move.dest < move.source){
                boardState.enPassantSquare = move.dest + 8;
            }
            else{
                boardState.enPassantSquare = move.dest - 8;
            }
            break;

        case KINGCASTLE:
            //Adding piece to dest square
            boardState.zobristHash ^= zobristTable[zobristPieceIndex][move.dest];
            setBit(boardState.pieces[boardState.sideToMove][pieceType], move.dest); 
            boardState.pieceArray[move.dest] = pieceType;
            //Move the h rook to f
            boardState.pieceArray[move.source + 3] = -1;
            boardState.pieceArray[move.source + 1] = ROOK;
            clearBit(boardState.pieces[boardState.sideToMove][ROOK], move.source + 3);
            setBit(boardState.pieces[boardState.sideToMove][ROOK], move.source + 1);
            zobristPieceIndex = ROOK + (6 * boardState.sideToMove);
            boardState.zobristHash ^= zobristTable[zobristPieceIndex][move.source + 3];
            boardState.zobristHash ^= zobristTable[zobristPieceIndex][move.source + 1];

            break;

        case QUEENCASTLE:
            //Adding piece to dest square
            boardState.zobristHash ^= zobristTable[zobristPieceIndex][move.dest];
            setBit(boardState.pieces[boardState.sideToMove][pieceType], move.dest); 
            boardState.pieceArray[move.dest] = pieceType;
            //Move the a rook to d
            boardState.pieceArray[move.source - 4] = -1;
            boardState.pieceArray[move.source - 1] = ROOK;
            clearBit(boardState.pieces[boardState.sideToMove][ROOK], move.source - 4);
            setBit(boardState.pieces[boardState.sideToMove][ROOK], move.source - 1);
            zobristPieceIndex = ROOK + (6 * boardState.sideToMove);
            boardState.zobristHash ^= zobristTable[zobristPieceIndex][move.source - 4];
            boardState.zobristHash ^= zobristTable[zobristPieceIndex][move.source - 1];
            break;

        case CAPTUREMOVE:
            //Adding piece to dest square
            boardState.zobristHash ^= zobristTable[zobristPieceIndex][move.dest];
            setBit(boardState.pieces[boardState.sideToMove][pieceType], move.dest); 
            boardState.pieceArray[move.dest] = pieceType;
            clearBit(boardState.pieces[opposingColor][destPieceType], move.dest);
            zobristPieceIndex = destPieceType + (6 * opposingColor);
            boardState.zobristHash ^= zobristTable[zobristPieceIndex][move.dest];
            break;

        case ENPASSANTCAPTURE:
            //Adding piece to dest square
            boardState.zobristHash ^= zobristTable[zobristPieceIndex][move.dest];
            setBit(boardState.pieces[boardState.sideToMove][pieceType], move.dest); 
            zobristPieceIndex = PAWN + (6 * opposingColor);
            boardState.pieceArray[move.dest] = pieceType;
            if(boardState.sideToMove == WHITE){
                boardState.pieceArray[move.dest - 8] = -1;
                clearBit(boardState.pieces[opposingColor][PAWN], move.dest - 8);
                boardState.zobristHash ^= zobristTable[zobristPieceIndex][move.dest - 8];
            }
            else{
                boardState.pieceArray[move.dest + 8] = -1;
                clearBit(boardState.pieces[opposingColor][PAWN], move.dest + 8);
                boardState.zobristHash ^= zobristTable[zobristPieceIndex][move.dest + 8];
            }
            break;

        case KNIGHTPROMO:
            boardState.pieceArray[move.dest] = KNIGHT;
            zobristPieceIndex = KNIGHT + (boardState.sideToMove * 6);
            //Adding piece to dest square
            boardState.zobristHash ^= zobristTable[zobristPieceIndex][move.dest];
            setBit(boardState.pieces[boardState.sideToMove][KNIGHT], move.dest);
            break;

        case BISHOPPROMO:
            boardState.pieceArray[move.dest] = BISHOP;
            zobristPieceIndex = BISHOP + (boardState.sideToMove * 6);
            //Adding piece to dest square
            boardState.zobristHash ^= zobristTable[zobristPieceIndex][move.dest];
            setBit(boardState.pieces[boardState.sideToMove][BISHOP], move.dest);
            break;

        case ROOKPROMO:
            boardState.pieceArray[move.dest] = ROOK;
            zobristPieceIndex = ROOK + (boardState.sideToMove * 6);
            //Adding piece to dest square
            boardState.zobristHash ^= zobristTable[zobristPieceIndex][move.dest];
            setBit(boardState.pieces[boardState.sideToMove][ROOK], move.dest);
            break;

        case QUEENPROMO:
            boardState.pieceArray[move.dest] = QUEEN;
            zobristPieceIndex = QUEEN + (boardState.sideToMove * 6);
            //Adding piece to dest square
            boardState.zobristHash ^= zobristTable[zobristPieceIndex][move.dest];
            setBit(boardState.pieces[boardState.sideToMove][QUEEN], move.dest);
            break;

        case KNIGHTPROMOCAPTURE:
            boardState.pieceArray[move.dest] = KNIGHT;
            zobristPieceIndex = KNIGHT + (boardState.sideToMove * 6);
            //Adding piece to dest square
            boardState.zobristHash ^= zobristTable[zobristPieceIndex][move.dest];
            zobristPieceIndex = destPieceType + (opposingColor * 6);
            boardState.zobristHash ^= zobristTable[zobristPieceIndex][move.dest];
            setBit(boardState.pieces[boardState.sideToMove][KNIGHT], move.dest);
            clearBit(boardState.pieces[opposingColor][destPieceType], move.dest);
            break;

        case BISHOPPROMOCAPTURE:
            boardState.pieceArray[move.dest] = BISHOP;
            zobristPieceIndex = BISHOP + (boardState.sideToMove * 6);
            //Adding piece to dest square
            boardState.zobristHash ^= zobristTable[zobristPieceIndex][move.dest];
            zobristPieceIndex = destPieceType + (opposingColor * 6);
            boardState.zobristHash ^= zobristTable[zobristPieceIndex][move.dest];
            setBit(boardState.pieces[boardState.sideToMove][BISHOP], move.dest);
            clearBit(boardState.pieces[opposingColor][destPieceType], move.dest);
            break;

        case ROOKPROMOCAPTURE:
            boardState.pieceArray[move.dest] = ROOK;
            zobristPieceIndex = ROOK + (boardState.sideToMove * 6);
            //Adding piece to dest square
            boardState.zobristHash ^= zobristTable[zobristPieceIndex][move.dest];
            zobristPieceIndex = destPieceType + (opposingColor * 6);
            boardState.zobristHash ^= zobristTable[zobristPieceIndex][move.dest];
            setBit(boardState.pieces[boardState.sideToMove][ROOK], move.dest);
            clearBit(boardState.pieces[opposingColor][destPieceType], move.dest);
            break;

        case QUEENPROMOCAPTURE:
            boardState.pieceArray[move.dest] = QUEEN;
            zobristPieceIndex = QUEEN + (boardState.sideToMove * 6);
            //Adding piece to dest square
            boardState.zobristHash ^= zobristTable[zobristPieceIndex][move.dest];
            zobristPieceIndex = destPieceType + (opposingColor * 6);
            boardState.zobristHash ^= zobristTable[zobristPieceIndex][move.dest];
            setBit(boardState.pieces[boardState.sideToMove][QUEEN], move.dest);
            clearBit(boardState.pieces[opposingColor][destPieceType], move.dest);
            break;

    }
    populateOccupiedSquares(boardState);
    boardState.sideToMove = boardState.sideToMove == WHITE ? BLACK : WHITE;
    zobristFlags(boardState);
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
            square -= 16;
            i++;
            continue;
        }
        if(fen[i] >= '1' && fen[i] <= '8'){
            square += (fen[i] - '0'); // Skip empty squares
            i++;
            continue;
        }
        switch (fen[i]){
            case 'r':
                setBit(boardState.pieces[BLACK][ROOK], square);
                boardState.pieceArray[square] = ROOK;
                break;
            case 'b':
                setBit(boardState.pieces[BLACK][BISHOP], square);
                boardState.pieceArray[square] = BISHOP;
                break;
            case 'n':
                setBit(boardState.pieces[BLACK][KNIGHT], square);
                boardState.pieceArray[square] = KNIGHT;
                break;
            case 'q':
                setBit(boardState.pieces[BLACK][QUEEN], square);
                boardState.pieceArray[square] = QUEEN;
                break;
            case 'k':
                setBit(boardState.pieces[BLACK][KING], square);
                boardState.pieceArray[square] = KING;
                break;
            case 'p':
                setBit(boardState.pieces[BLACK][PAWN], square);
                boardState.pieceArray[square] = PAWN;
                break;
            case 'R':
                setBit(boardState.pieces[WHITE][ROOK], square);
                boardState.pieceArray[square] = ROOK;
                break;
            case 'B':
                setBit(boardState.pieces[WHITE][BISHOP], square);
                boardState.pieceArray[square] = BISHOP;
                break;
            case 'N':
                setBit(boardState.pieces[WHITE][KNIGHT], square);
                boardState.pieceArray[square] = KNIGHT;
                break;
            case 'Q':
                setBit(boardState.pieces[WHITE][QUEEN], square);
                boardState.pieceArray[square] = QUEEN;
                break;
            case 'K':
                setBit(boardState.pieces[WHITE][KING], square);
                boardState.pieceArray[square] = KING;
                break;
            case 'P':
                setBit(boardState.pieces[WHITE][PAWN], square);
                boardState.pieceArray[square] = PAWN;
                break;
                
        }
        square++;
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
        i++;
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
    boardState.zobristHash = boardStateHash(boardState);
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

std::string squareToAlgebraic(int sq) {
    std::string s = "";
    s += (char)('a' + (sq % 8));
    s += (char)('1' + (sq / 8));
    return s;
}

uint64_t perft(BoardState& boardState, int depth) {
    // Base case: If we reached the target depth, this is 1 valid board state
    if (depth == 0) {
        return 1ULL;
    }

    uint64_t nodes = 0;
    MoveList pseudoMoves = generateMoves(boardState, boardState.sideToMove);
    int myColor = boardState.sideToMove;
    int oppColor = (myColor == WHITE) ? BLACK : WHITE;

    for (int i = 0; i < pseudoMoves.count; i++) {
        Move move = pseudoMoves.moves[i];
        UndoState undo;

        // 1. Make the pseudo-legal move
        makeMove(boardState, move, undo);

        // 2. Find our king and verify he wasn't left in check
        int myKingSquare = __builtin_ctzll(boardState.pieces[myColor][KING]);

        if (!isSquareAttacked(boardState, myKingSquare, oppColor)) {
            // 3. Move is legal! Recursively step down the tree
            nodes += perft(boardState, depth - 1);
        }

        // 4. Restore the board
        unmakeMove(boardState, move, undo);
    }

    return nodes;
}

void perftDivide(BoardState& boardState, int depth) {
    uint64_t totalNodes = 0;
    MoveList pseudoMoves = generateMoves(boardState, boardState.sideToMove);
    int myColor = boardState.sideToMove;
    int oppColor = (myColor == WHITE) ? BLACK : WHITE;

    std::cout << "--- Perft Divide Depth " << depth << " ---" << std::endl;

    for (int i = 0; i < pseudoMoves.count; i++) {
        Move move = pseudoMoves.moves[i];
        UndoState undo;

        makeMove(boardState, move, undo);

        int myKingSquare = __builtin_ctzll(boardState.pieces[myColor][KING]);
        if (!isSquareAttacked(boardState, myKingSquare, oppColor)) {
            // Run the recursive perft for this specific move
            uint64_t nodes = perft(boardState, depth - 1);

            // Print the branch result
            std::cout << squareToAlgebraic(move.source) << squareToAlgebraic(move.dest)
                      << ": " << nodes << std::endl;

            totalNodes += nodes;
        }

        unmakeMove(boardState, move, undo);
    }

    std::cout << "\nTotal Nodes: " << totalNodes << std::endl;
}

//move.raw will be 0 if non-valid move
Move strMoveToMove(const std::string& strMove, BoardState& boardState){
    Move move;
    move.raw = 0;

    if(strMove.length() < 4){
        return move;
    }

    int srcX = strMove[0] - 'a';
    int srcY = strMove[1] - '1';
    int destX = strMove[2] - 'a';
    int destY = strMove[3] - '1';

    int srcIndex = srcY * 8 + srcX; 
    int destIndex = destY * 8 + destX;

    // Determine if this is a promotion move
    uint8_t targetPromoFlag = 0;
    bool isPromotion = (strMove.length() >= 5);

    if (isPromotion) {
        char promoChar = strMove[4];
        if (promoChar == 'q'){ 
            targetPromoFlag = QUEENPROMO;
        }
        else if (promoChar == 'r'){
            targetPromoFlag = ROOKPROMO;
        }
        else if (promoChar == 'b'){
            targetPromoFlag = BISHOPPROMO;
        }
        else if (promoChar == 'n'){
            targetPromoFlag = KNIGHTPROMO;
        }
    }

    MoveList legalMoves = generateLegalMoves(boardState);

    for(int i = 0; i < legalMoves.count; i++){
        if(legalMoves.moves[i].source == srcIndex && legalMoves.moves[i].dest == destIndex){
            if(isPromotion){
                uint8_t moveFlag = legalMoves.moves[i].flags;
                if(moveFlag == targetPromoFlag || moveFlag == (targetPromoFlag | CAPTUREMOVE)){
                move = legalMoves.moves[i];
                break;
                } 
            }
            else{
                move = legalMoves.moves[i]; 
                break;
            }
        }
    }

    return move;
}

//2 color 6 pieces
char fenPiece[2][6] = {
    {'P', 'B', 'N', 'R', 'Q', 'K'},
    {'p', 'b', 'n', 'r', 'q', 'k'}
};

std::string boardStateToFen(BoardState& boardState){
    std::string fen;
    for(int y = 7; y >= 0; y--){
        int skipSquares = 0;
        for(int x = 0; x < 8; x++){
            int currentSquare = y * 8 + x;
            if(isOccupied(boardState.occupiedSquares[2], currentSquare)){
                int pieceType = PAWN;
                int pieceColor = WHITE;
                for(int color = WHITE; color <= BLACK; color++){
                    for(int piece = PAWN; piece <= KING; piece++){
                        if(isOccupied(boardState.pieces[color][piece], currentSquare)){
                            pieceType = piece;
                            pieceColor = color;
                        
                        }
                    } 
                }
                if(skipSquares != 0){
                    fen += std::to_string(skipSquares);
                    skipSquares = 0;
                }
                fen += fenPiece[pieceColor][pieceType];
            }
            else{
                skipSquares++;
            }
        }
        if(skipSquares != 0){
            fen += std::to_string(skipSquares);
        }
        fen += '/';
        
    }

    fen += ' ';
    if(boardState.sideToMove == WHITE){
        fen += 'w';
    }
    else{
        fen += 'b';
    }
    fen += ' ';

    if(boardState.castlingRights.kingSideCastleWhite){
        fen += 'K'; 
    }
    if(boardState.castlingRights.queenSideCastleWhite){
        fen += 'Q'; 
    }
    if(boardState.castlingRights.kingSideCastleBlack){
        fen += 'k'; 
    }
    if(boardState.castlingRights.queenSideCastleBlack){
        fen += 'q'; 
    }
    fen += ' ';

    if(boardState.enPassantSquare == -1){
        fen += '-';
    }
    else{
        fen += squareToAlgebraic(boardState.enPassantSquare);
    }

    return fen;
}

void moveSwap(MoveList legalMoves, Move possibleBestMove){
    for(int i = 0; i < legalMoves.count; i++){
        if(legalMoves.moves[i].raw == possibleBestMove.raw){
            std::swap(legalMoves.moves[0], legalMoves.moves[i]); 
            std::swap(legalMoves.moveScores[0], legalMoves.moveScores[i]);
            break;
        } 
    }
}

std::string promotionChar(Move move){
    if((move.flags & KNIGHTPROMO) == KNIGHTPROMO){
        return "n"; 
    }
    else if((move.flags & KNIGHTPROMO) == BISHOPPROMO){
        return "b"; 
    }
    else if((move.flags & KNIGHTPROMO) == ROOKPROMO){
        return "r"; 
    }
    else if((move.flags & KNIGHTPROMO) == QUEENPROMO){
        return "q"; 
    }
    else{
        return "";
    }
}

std::string createPositionCmd(BoardState& boardState){
    std::string positionCmd = "position fen ";
    positionCmd += boardStateToFen(boardState);
    /*if(currentMove == 0){
        return positionCmd;
    }
    positionCmd += " moves ";
    int i = 0;
    while(i < currentMove){
        std::string sourceSquare = squareToAlgebraic(moveHistory[i].source);
        std::string destSquare = squareToAlgebraic(moveHistory[i].dest);
        positionCmd += sourceSquare + destSquare;
        int flags = moveHistory[i].flags;
        if(flags == KNIGHTPROMO || flags == KNIGHTPROMOCAPTURE){
            positionCmd += "n";
        }
        else if(flags == BISHOPPROMO || flags == BISHOPPROMOCAPTURE){
            positionCmd += "b";
        }
        else if(flags == ROOKPROMO || flags == ROOKPROMOCAPTURE){
            positionCmd += "r";
        }
        else if(flags == QUEENPROMO || flags == QUEENPROMOCAPTURE){
            positionCmd += "q";
        }
        positionCmd +=  " ";
        i++;
    }*/

    return positionCmd;

}
