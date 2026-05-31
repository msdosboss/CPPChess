//g++ -o main main.cpp `sdl2-config --cflags --libs` -lSDL2_image
#include "main.hpp"

#define WIDTH 800
#define HEIGHT 800
#define COLLUMNS 8
#define ROWS 8
#define FPS 60

//2 for color and 6 for each piece
SDL_Texture *pieceTextures[2][6];
SDL_Rect board[8][8];

void initTexture(SDL_Renderer *rend){
    pieceTextures[WHITE][PAWN] = IMG_LoadTexture(rend, "img/whitePawn.png");
    pieceTextures[BLACK][PAWN] = IMG_LoadTexture(rend, "img/blackPawn.png");
    pieceTextures[WHITE][KNIGHT] = IMG_LoadTexture(rend, "img/whiteKnight.png");
    pieceTextures[BLACK][KNIGHT] = IMG_LoadTexture(rend, "img/blackKnight.png");
    pieceTextures[WHITE][BISHOP] = IMG_LoadTexture(rend, "img/whiteBishop.png");
    pieceTextures[BLACK][BISHOP] = IMG_LoadTexture(rend, "img/blackBishop.png");
    pieceTextures[WHITE][ROOK] = IMG_LoadTexture(rend, "img/whiteRook.png");
    pieceTextures[BLACK][ROOK] = IMG_LoadTexture(rend, "img/blackRook.png");
    pieceTextures[WHITE][QUEEN] = IMG_LoadTexture(rend, "img/whiteQueen.png");
    pieceTextures[BLACK][QUEEN] = IMG_LoadTexture(rend, "img/blackQueen.png");
    pieceTextures[WHITE][KING] = IMG_LoadTexture(rend, "img/whiteKing.png");
    pieceTextures[BLACK][KING] = IMG_LoadTexture(rend, "img/blackKing.png");
}

void initBoardRect(){
    for(int i = 0; i < 8; i++){
        for(int j = 0; j < 8; j++){
            board[i][j] = {(int) SIZE * i, (int) SIZE * j, SIZE, SIZE};
        }
    }
}

SDL_Window *initDisplay(int width, int height, int x, int y, Uint32 flag){
    /* Initializes the timer, audio, video, joystick,
    haptic, gamecontroller and events subsystems */
    if(SDL_Init(SDL_INIT_EVERYTHING) != 0){
        printf("Error initializing SDL: %s\n", SDL_GetError());
        return 0;
    }
    /* Create a window */
    SDL_Window* wind = 
        SDL_CreateWindow("Graggle Chess", x, y, width, height, flag);
    
    if(!wind){
        printf("Error creating window: %s\n", SDL_GetError());
        SDL_Quit();
        return 0;
    }

    return wind;
}

SDL_Renderer *initRender(SDL_Window *wind){
    /* Create a renderer */
    Uint32 render_flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
    SDL_Renderer* rend = SDL_CreateRenderer(wind, -1, render_flags);
    if (!rend){
        printf("Error creating renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(wind);
        SDL_Quit();
        return 0;
    }

    return rend;
}

void freeTextures(){
    for(int i = PAWN; i <= KING; i++){
        SDL_DestroyTexture(pieceTextures[WHITE][i]);
        SDL_DestroyTexture(pieceTextures[BLACK][i]);
    }
}


void highlightSelectedPiece(Uint32 *pixels, int pieceSelectedX, int pieceSelectedY){
    Uint32 pixelColor = 0 << 24 | 0 << 16 | 0 << 8 | 255;

    for(int i = (pieceSelectedX / 100) * 100; i < (pieceSelectedX / 100) * 100 + 100; i++){
        pixels[((pieceSelectedY / 100) * 100) * HEIGHT + i] = pixelColor;
        pixels[((pieceSelectedY / 100) * 100) * HEIGHT + i + 1] = pixelColor; 
        pixels[((pieceSelectedY / 100) * 100 + 99) * HEIGHT + i] = pixelColor;
        pixels[((pieceSelectedY / 100) * 100 + 98) * HEIGHT + i] = pixelColor;
    }
    for(int i = (pieceSelectedY / 100) * 100; i < (pieceSelectedY / 100) * 100 + 100; i++){
        pixels[(pieceSelectedX / 100) * 100 + i * WIDTH] = pixelColor;
        pixels[(pieceSelectedX / 100) * 100 + 1 + i * WIDTH] = pixelColor;
        pixels[((pieceSelectedX / 100) * 100 + 99) + i * WIDTH] = pixelColor;
        pixels[((pieceSelectedX / 100) * 100 + 98) + i * WIDTH] = pixelColor;
    }
}

uint8_t promoWindow(int color){
    SDL_Window *wind = initDisplay(400, 100, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED + 500, SDL_WINDOW_INPUT_GRABBED);

    SDL_Renderer *rend = initRender(wind);
    
    SDL_Texture *backgroundTexture = SDL_CreateTexture(rend, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, 400, 100);
    
    Uint32 *pixels = new Uint32[400 * 100];

    SDL_Event event;

    if(color == BLACK){
        for(int i = 0; i < 400 * 100; i++){
            pixels[i] = 255 << 24 | 255 << 16 | 255 << 8 | 255;
        }
        SDL_UpdateTexture(backgroundTexture, NULL, pixels, sizeof(Uint32) * 400);
        SDL_RenderCopy(rend, backgroundTexture, NULL, NULL);
    }

    for(int i = 0; i < 4; i++){
        SDL_RenderCopy(rend, pieceTextures[color][KNIGHT + i], NULL, &(board[i][0])); // Layout horizontally
    }

    SDL_RenderPresent(rend);

    while(1){
        while(SDL_PollEvent(&event)){
            switch(event.type){
                case SDL_MOUSEBUTTONDOWN:
                    SDL_DestroyTexture(backgroundTexture);
                    delete[] pixels;
                    SDL_DestroyRenderer(rend);
                    SDL_DestroyWindow(wind);
                    switch(event.button.x / 100){
                        case 0:
                            return QUEENPROMO;
                        case 1:
                            return ROOKPROMO;
                        case 2:
                            return BISHOPPROMO;
                        case 3:
                            return KNIGHTPROMO; 
                    }
            }
        }
    }
}

int displayLoop(SDL_Window *wind, SDL_Renderer *rend){

    SDL_Event event;

    SDL_Texture *boardTexture = SDL_CreateTexture(rend, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, WIDTH, HEIGHT);

    int running = 1;

    Uint32 *pixels = new Uint32[WIDTH * HEIGHT];

    int pieceSelectedState = 0;    //0 = no piece clicked, 1 = piece clicked, 2 = move selected
    int pieceSelectedX = 0;
    int pieceSelectedY = 0;
    
    int sourceIndex = -1;
    int destIndex = -1; 

    BoardState boardState;
    fenToBoardState("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq -", boardState);
    //fenToBoardState("r1bqk2r/ppppbppp/2nn4/1B2N3/8/8/PPPP1PPP/RNBQR1K1 w kq - 1 7", boardState);

    while(running){
        while(SDL_PollEvent(&event)){
            switch(event.type){
                case SDL_QUIT:
                    running = 0;
                    break;
                case SDL_MOUSEBUTTONDOWN: {
                    // Calculate the square INSIDE the event loop before the data is lost
                    int clickX = event.button.x / 100;
                    int clickY = event.button.y / 100;
                    int mappedY = 7 - clickY;
                    int clickedIndex = clickX + (mappedY * 8);

                    if(!pieceSelectedState){
                        // Only transition to state 1 if the current player actually clicked on their own piece
                        if(isOccupied(boardState.occupiedSquares[boardState.sideToMove], clickedIndex)){
                            pieceSelectedState = 1;
                            sourceIndex = clickedIndex;
                            pieceSelectedX = clickX; // Used for drawing the highlight
                            pieceSelectedY = clickY; 
                        }
                    }
                    else{
                        // The user clicked again, this is their destination square
                        pieceSelectedState = 2;
                        destIndex = clickedIndex;
                    }
                    break;
                }
            }
        } 

        int isMoveLegal = 0;
    
        if(pieceSelectedState == 2 && destIndex != -1){ // Verify we actually captured a destination

            MoveList legalMoves = generateLegalMoves(boardState);
            Move matchedMove;
            matchedMove.raw = 0;

            for(int i = 0; i < legalMoves.count; i++){
                if(legalMoves.moves[i].source == sourceIndex && legalMoves.moves[i].dest == destIndex){
                    matchedMove = legalMoves.moves[i];

                    // Handle Promotion Selection
                    if(matchedMove.flags >= KNIGHTPROMO){
                        uint8_t promoChoice = promoWindow(boardState.sideToMove);
                        // Loop forward to find the exact promotion flag they chose
                        while(i < legalMoves.count && 
                              legalMoves.moves[i].source == sourceIndex && 
                              legalMoves.moves[i].dest == destIndex) {
                            
                            if(legalMoves.moves[i].flags == promoChoice || 
                               legalMoves.moves[i].flags == (promoChoice | CAPTUREMOVE)) {
                                matchedMove = legalMoves.moves[i];
                                break;
                            }
                            i++;
                        }
                    }
                    break; // Move found and verified
                }
            }

            // If a valid matching move was found, apply it
            if(matchedMove.raw != 0){
                UndoState undo;
                makeMove(boardState, matchedMove, undo);
                
                isMoveLegal = 1;
            }
            
            // Reset state variables waiting for the next user click
            pieceSelectedState = 0;
            sourceIndex = -1;
            destIndex = -1;
        }

        for(int i = 0; i < WIDTH; i++){
            for(int j = 0; j < HEIGHT; j++){
                if((((j + 1) / 100) % 2 != 0 && ((i + 1) / 100) % 2 != 0) || (((j + 1) / 100) % 2 == 0 && ((i + 1) / 100) % 2 == 0)){
                    pixels[j + i * WIDTH] = 240 << 24 | 239 << 16 | 194 << 8 | 255;    
                }
                else{
                    pixels[j + i * WIDTH] = 238 << 24 | 115 << 16 | 118 << 8 | 255;
                }
            }
        }

        if(pieceSelectedState == 1 && sourceIndex != -1){
            highlightSelectedPiece(pixels, pieceSelectedX * 100, pieceSelectedY * 100);    
        }
        else{
            pieceSelectedState = 0;
        }

        SDL_UpdateTexture(boardTexture, NULL, pixels, sizeof(Uint32) * WIDTH);
        SDL_RenderCopy(rend, boardTexture, NULL, NULL);
        
        // Render the current bitboards to the screen
        for(int rendColor = WHITE; rendColor <= BLACK; rendColor++){
            for(int i = PAWN; i <= KING; i++){
                uint64_t pieceBoard = boardState.pieces[rendColor][i];
                while(pieceBoard != 0){
                    int currentPiece = __builtin_ctzll(pieceBoard);
                    int drawX = currentPiece % 8;
                    int drawY = 7 - (currentPiece / 8);

                    SDL_RenderCopy(rend, pieceTextures[rendColor][i], NULL, &(board[drawX][drawY]));
                    clearBit(pieceBoard, currentPiece);
                }
            }
        }

        /* Draw to window and loop */
        SDL_RenderPresent(rend);
        SDL_Delay(1000/FPS);
    }

    /* Release resources */
    SDL_DestroyTexture(boardTexture);
    freeTextures();
    SDL_DestroyRenderer(rend);
    SDL_DestroyWindow(wind);
    delete[] pixels;
    return 0;
}

int main(){ 
    SDL_Window *wind = initDisplay(WIDTH, HEIGHT, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 0);
    SDL_Renderer *rend = initRender(wind);
    
    initBoardRect(); 
    initTexture(rend);
    
    // Precompute physics lookup tables
    generateKingAttacks();
    generateKnightAttacks();

    displayLoop(wind, rend);
    return 0;
}
