#include "gui.hpp"

//2 for color and 6 for each piece
SDL_Texture *pieceTextures[2][6];
SDL_Texture *dotTexture;

void initTexture(SDL_Renderer *rend, std::string textureDir){
    //Check if tailing / is part of textureDir
    //Add if not
    if(textureDir[textureDir.length() - 1] != '/'){
        textureDir += "/";
    }

    pieceTextures[WHITE][PAWN] = IMG_LoadTexture(rend,  (textureDir + "whitePawn.png").c_str());
    pieceTextures[BLACK][PAWN] = IMG_LoadTexture(rend, (textureDir + "blackPawn.png").c_str());
    pieceTextures[WHITE][KNIGHT] = IMG_LoadTexture(rend, (textureDir + "whiteKnight.png").c_str());
    pieceTextures[BLACK][KNIGHT] = IMG_LoadTexture(rend, (textureDir + "blackKnight.png").c_str());
    pieceTextures[WHITE][BISHOP] = IMG_LoadTexture(rend, (textureDir + "whiteBishop.png").c_str());
    pieceTextures[BLACK][BISHOP] = IMG_LoadTexture(rend, (textureDir + "blackBishop.png").c_str());
    pieceTextures[WHITE][ROOK] = IMG_LoadTexture(rend, (textureDir + "whiteRook.png").c_str());
    pieceTextures[BLACK][ROOK] = IMG_LoadTexture(rend, (textureDir + "blackRook.png").c_str());
    pieceTextures[WHITE][QUEEN] = IMG_LoadTexture(rend, (textureDir + "whiteQueen.png").c_str());
    pieceTextures[BLACK][QUEEN] = IMG_LoadTexture(rend, (textureDir + "blackQueen.png").c_str());
    pieceTextures[WHITE][KING] = IMG_LoadTexture(rend, (textureDir + "whiteKing.png").c_str());
    pieceTextures[BLACK][KING] = IMG_LoadTexture(rend, (textureDir + "blackKing.png").c_str());
    dotTexture = IMG_LoadTexture(rend, (textureDir + "dot.png").c_str());
}

void freeTextures(){
    for(int i = PAWN; i <= KING; i++){
        SDL_DestroyTexture(pieceTextures[WHITE][i]);
        SDL_DestroyTexture(pieceTextures[BLACK][i]);
    }
    SDL_DestroyTexture(dotTexture);
}

void renderBoard(
        BoardState& boardState, 
        std::atomic<bool>& gameOver,
        uint32_t squareDarkColor,
        uint32_t squareLightColor,
        std::string textureDir,
        int playerSide,
        std::string& moveMadeStr,
        std::mutex& m,
        std::condition_variable& cv
    ){
    if(SDL_Init(SDL_INIT_VIDEO) < 0){
        std::cerr << "SDL cound not init SDL_ERROR: " << SDL_GetError() << std::endl;
        return;
    }

    SDL_Window *window = SDL_CreateWindow(
        "Game Viewer",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI
    );

    if(window == NULL){
        std::cerr << "Window cound not be created SDL_ERROR: " << SDL_GetError() << std::endl; 
        return;
    }

    SDL_Renderer *rend = SDL_CreateRenderer(
        window,
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC        
    );

    if(rend == NULL){
        std::cerr << "Failed to create renderer SDL_ERROR: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return;
    }

    uint8_t squareDarkRed = squareDarkColor & 0xff;
    uint8_t squareDarkBlue = (squareDarkColor & 0xff00) >> 8;
    uint8_t squareDarkGreen = (squareDarkColor & 0xff0000) >> 16;
    uint8_t squareDarkAlpha = (squareDarkColor & 0xff000000) >> 24;
    uint8_t squareLightRed = squareLightColor & 0xff;
    uint8_t squareLightBlue = (squareLightColor & 0xff00) >> 8;
    uint8_t squareLightGreen = (squareLightColor & 0xff0000) >> 16;
    uint8_t squareLightAlpha = (squareLightColor & 0xff000000) >> 24;
    
    SDL_Event event;
    uint32_t frameStart;
    int frameTime;
    SDL_Rect squares[8][8];
    initTexture(rend, textureDir);
    for(int squareXIndex = 0; squareXIndex < 8; squareXIndex++){ 
        for(int squareYIndex = 0; squareYIndex < 8; squareYIndex++){ 
            squares[squareXIndex][squareYIndex] = {
                .x = squareXIndex * SQUARE_WIDTH,
                .y = squareYIndex * SQUARE_HEIGHT,
                .w = SQUARE_WIDTH,
                .h = SQUARE_HEIGHT
            }; 
        }
    }

    int pieceSelectedState = 0;
    int clickX, clickY, mappedY;
    clickX = 0;
    clickY = 0;
    mappedY = 0;
    int sourceIndex = -1;
    int destIndex = -1;
    int pieceSelectedX = 0;
    int pieceSelectedY = 0;
    while(!gameOver){
        frameStart = SDL_GetTicks();

        while(SDL_PollEvent(&event) != 0){
            if(event.type == SDL_QUIT){
                gameOver = true;
            }
            else if(event.type == SDL_KEYDOWN){
                if(event.key.keysym.sym == SDLK_ESCAPE){
                    gameOver = true;
                }
            }
            else if(event.type == SDL_MOUSEBUTTONDOWN){
                clickX = event.button.x / (int)SQUARE_WIDTH;
                clickY = event.button.y / (int)SQUARE_HEIGHT;
                mappedY = 7 - clickY;
                int clickedIndex = clickX + (mappedY * 8);
                std::unique_lock lk(m);
                if(!pieceSelectedState){
                    if(isOccupied(boardState.occupiedSquares[boardState.sideToMove], clickedIndex)){
                        pieceSelectedState = 1;
                        sourceIndex = clickedIndex;
                        pieceSelectedX = clickX;
                        pieceSelectedY = clickY;
                    }
                    else{
                        pieceSelectedState = 0;
                    }
                }
                else{
                    if(boardState.sideToMove == playerSide){
                        pieceSelectedState = 2;
                        destIndex = clickedIndex;
                    }
                    else{
                        pieceSelectedState = 0;
                    }
                }
                lk.unlock();
            }
        }
        std::unique_lock lk(m);
        if(playerSide == boardState.sideToMove && pieceSelectedState == 2 && destIndex != -1){
            MoveList legalMoves = generateLegalMoves(boardState);
            for(int i = 0; i < legalMoves.count; i++){
                //Still need to do something about pawn promo
                if(legalMoves.moves[i].source == sourceIndex && legalMoves.moves[i].dest == destIndex){
                    Move selectedMove = legalMoves.moves[i];
                    UndoState undoState;
                    makeMove(boardState, selectedMove, undoState);
                    //Converting move to UCI move string
                    char f1 = 'a' + (sourceIndex % 8);
                    char r1 = '1' + (sourceIndex / 8);
                    char f2 = 'a' + (destIndex % 8);
                    char r2 = '1' + (destIndex / 8);
                    moveMadeStr = std::string(1, f1) + r1 + f2 + r2;

                    if((selectedMove.flags & KNIGHTPROMO) == KNIGHTPROMO){
                        moveMadeStr += 'n';
                    }
                    else if((selectedMove.flags & BISHOPPROMO) == BISHOPPROMO){
                        moveMadeStr += 'b';
                    }
                    else if((selectedMove.flags & ROOKPROMO) == ROOKPROMO){
                        moveMadeStr += 'r';
                    }
                    else if((selectedMove.flags & QUEENPROMO) == QUEENPROMO){
                        moveMadeStr += 'q';
                    }

                    legalMoves = generateLegalMoves(boardState);
                    if(legalMoves.count == 0){
                        gameOver = true;
                    }
                    //Tell the engine to make its move
                    std::cerr << "moveMadeStr: " << moveMadeStr << std::endl;
                    cv.notify_all();
                    break;
                } 
            }
            destIndex = -1;
            pieceSelectedState = 0;
        }
        lk.unlock();
        //Render squares on board
        for(int x = 0; x < 8; x++){
            for(int y = 0; y < 8; y++){
                if(y % 2 == 0){
                    if(x % 2 == 0){
                        SDL_SetRenderDrawColor(
                            rend, 
                            squareLightRed, 
                            squareLightBlue, 
                            squareLightGreen, 
                            squareLightAlpha
                        );
                    }
                    else{
                        SDL_SetRenderDrawColor(
                            rend, 
                            squareDarkRed, 
                            squareDarkBlue, 
                            squareDarkGreen, 
                            squareDarkAlpha
                        );
                    }
                }
                else{
                    if(x % 2 == 0){
                        SDL_SetRenderDrawColor(
                            rend, 
                            squareDarkRed, 
                            squareDarkBlue, 
                            squareDarkGreen, 
                            squareDarkAlpha
                        );
                    }
                    else{
                        SDL_SetRenderDrawColor(
                            rend, 
                            squareLightRed, 
                            squareLightBlue, 
                            squareLightGreen, 
                            squareLightAlpha
                        );
                    } 
                }
                SDL_RenderFillRect(rend, &(squares[x][y]));
            }
        }
        lk.lock();
        BoardState boardStateCopy = boardState;
        lk.unlock();
        // Render the current bitboards to the screen
        for(int rendColor = WHITE; rendColor <= BLACK; rendColor++){
            for(int i = PAWN; i <= KING; i++){
                uint64_t pieceBoard = boardStateCopy.pieces[rendColor][i];
                while(pieceBoard != 0){
                    int currentPiece = __builtin_ctzll(pieceBoard);
                    int drawX = currentPiece % 8;
                    int drawY = 7 - (currentPiece / 8);

                    SDL_RenderCopy(rend, pieceTextures[rendColor][i], NULL, &(squares[drawX][drawY]));
                    clearBit(pieceBoard, currentPiece);
                }
            }
        }
        //Render legal moves as dots on the squares
        if(pieceSelectedState == 1){
            //Outline the selectedSquare red for now, probably should pass the color to renderBoard
            SDL_SetRenderDrawColor(rend, 255, 0, 0, 255);
            SDL_RenderDrawRect(rend, &(squares[pieceSelectedX][pieceSelectedY]));
            MoveList legalMoves = generateLegalMoves(boardStateCopy);
            for(int i = 0; i < legalMoves.count; i++){
                if(legalMoves.moves[i].source == sourceIndex){
                    int moveXDst = legalMoves.moves[i].dest % 8;
                    int moveYDst = legalMoves.moves[i].dest / 8;
                    int mappedYDst = 7 - moveYDst;
                    SDL_RenderCopy(rend, dotTexture, NULL, &(squares[moveXDst][mappedYDst]));
                }
            }
        }

        SDL_RenderPresent(rend);
        frameTime = SDL_GetTicks() - frameStart;
        if (frameTime < FRAME_DELAY) {
            SDL_Delay(FRAME_DELAY - frameTime);
        }
    }
    freeTextures();
    SDL_DestroyRenderer(rend);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
