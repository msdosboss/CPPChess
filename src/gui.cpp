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



void renderBoard(
        BoardState& boardState, 
        std::atomic<bool>& isRunning,
        uint32_t squareDarkColor,
        uint32_t squareLightColor,
        std::string textureDir,
        std::mutex& m
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

    while(isRunning){
        frameStart = SDL_GetTicks();

        while(SDL_PollEvent(&event) != 0){
            if(event.type == SDL_QUIT){
                isRunning = false;
            }
            else if(event.type == SDL_KEYDOWN){
                if(event.key.keysym.sym == SDLK_ESCAPE){
                    isRunning = false;
                }
            }
        }
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
        std::unique_lock lk(m);
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

        SDL_RenderPresent(rend);
        frameTime = SDL_GetTicks() - frameStart;
        if (frameTime < FRAME_DELAY) {
            SDL_Delay(FRAME_DELAY - frameTime);
        }
    }
    SDL_DestroyRenderer(rend);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
