//g++ -o main main.cpp `sdl2-config --cflags --libs` -lSDL2_image
#include "UCIClient.hpp"

Move moveHistory[MAXMOVESPERGAME];
UndoState undoHistory[MAXMOVESPERGAME];
int currentMove = 0;


/*uint8_t promoWindow(int color){
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
}*/

void engineThread(
    BoardState& boardState,
    std::atomic<bool>& gameOver,
    int color,
    EngineProcess& engine,
    std::mutex& m,
    std::condition_variable& cv
){
    bool engineThinking = false;
    while(true){
        std::unique_lock lk(m);
        cv.wait(lk, [&gameOver, &boardState, &color]{
            return gameOver || boardState.sideToMove == color; 
        }); 
        if(gameOver){
            return;
        }
        if(!engineThinking){
            std::string positionCmd = createPositionCmd(boardState);
            engine.sendCommand(positionCmd);
            //engine.sendCommand("go wtime 100000 btime 100000");
            engine.sendCommand("go");
            //engine.sendCommand("go depth 8");
            engineThinking = true;
        }
        else if(engine.hasData()){
            std::string engineResponse = engine.receiveCommand();
            std::cout << "Engine says: " << engineResponse << std::endl;

            if(engineResponse.find("bestmove") != std::string::npos){
                std::istringstream ss(engineResponse);
                std::string token;
                //Skip "bestmove"
                ss >> token;
                //token equals move ie "e2e4"
                ss >> token;

                Move engineMove = strMoveToMove(token, boardState);

                //MoveList legalMoves = generateLegalMoves(boardState);
                UndoState undo;
                makeMove(boardState, engineMove, undo);
                moveHistory[currentMove] = engineMove;
                undoHistory[currentMove] = undo;
                currentMove++;

                engineThinking = false;
                MoveList legalMoves = generateLegalMoves(boardState);
                if(legalMoves.count == 0){
                    gameOver = true;
                }
            }
        }
    }
}


int main(){ 
    int playerColor = WHITE;
    
    // Precompute physics lookup tables
    generateKingAttacks();
    generateKnightAttacks();
    initZobristTable("data/zobristKeys.json");
    loadOpeningBook("data/openBook.json");

    EngineProcess engine("./engine");
    std::thread userEngineThread;
    //EngineProcess engine("otherEngines/stockfish/stockfish");

    if (!engine.initCommunication()) {
        std::cerr << "Failed to establish UCI communication with engine" << std::endl;
        return -1;
    }
    BoardState boardState;
    fenToBoardState(STARTFEN, boardState);
    //Caused a crash in matchManager but does not crash when given to the engine IDK
    //fenToBoardState("3r2k1/4R2p/3p4/1Ppr4/6P1/8/2R2K1P/8 w  -", boardState);
    //fenToBoardState("r2q1r2/ppp3k1/2np3Q/8/2BPPp2/2N5/PPP5/2K4R b  -", boardState);
    //fenToBoardState("r4kQ1/1pqb1r1p/4pB2/p2pPp2/P1pP4/P1P3R1/2B2PP1/4K1N1 b  -", boardState);
    //Slow middle game position
    //fenToBoardState("4rk1r/2pnq1pp/1p1bQn2/p3P3/3P1N2/2N5/PP1B1PPP/R3K2R w KQ - 1 16", boardState);
    //Rook King end game
    //fenToBoardState("8/8/8/4k3/8/8/3K4/3R4 w - - 0 1", boardState);
    //fenToBoardState("r1bqkb1r/pp2pppp/2np1n2/8/2BNP3/2N5/PPP2PPP/R1BQK2R w KQkq - 0 1", boardState);

    //perftDivide(boardState, 5);

    std::atomic<bool> gameOver = false;
    std::mutex threadMutex;
    std::condition_variable mutexCondition;
    int engineColor = playerColor == WHITE ? BLACK : WHITE;
    
    userEngineThread = std::thread(
        engineThread,
        std::ref(boardState),
        std::ref(gameOver),
        engineColor,
        std::ref(engine),
        std::ref(threadMutex),
        std::ref(mutexCondition)
    );

    uint32_t lightColor = 0xffffffff;
    uint32_t darkColor = 0xff4a9627;
    std::string moveMade;
    renderBoard(
        std::ref(boardState),
        std::ref(gameOver),
        darkColor,
        lightColor,
        "img",
        playerColor,
        std::ref(moveMade),
        std::ref(threadMutex),
        std::ref(mutexCondition)
    );

    gameOver = true;
    mutexCondition.notify_all();
    if(userEngineThread.joinable()){
        userEngineThread.join();
    }

    return 0;
}
