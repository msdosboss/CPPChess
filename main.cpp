//g++ -o main main.cpp `sdl2-config --cflags --libs` -lSDL2_image
#include "piece.h"

#define WIDTH 800
#define HEIGHT 800
#define COLLUMNS 8
#define ROWS 8
#define FPS 60

SDL_Window *initDisplay(int width, int height, int x, int y, Uint32 flag){
	/* Initializes the timer, audio, video, joystick,
	haptic, gamecontroller and events subsystems */
	if(SDL_Init(SDL_INIT_EVERYTHING) != 0){
		printf("Error initializing SDL: %s\n", SDL_GetError());
		return 0;
	}
	/* Create a window */
	SDL_Window* wind = SDL_CreateWindow("Graggle Chess",
				      x,
				      y,
				      width, height, flag);
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

Piece ***fenToBoard(const char *FEN, SDL_Renderer *rend, int *color){
	Piece ***board = new Piece**[ROWS];
	for(int i = 0; i < ROWS; i++){
		board[i] = new Piece*[COLLUMNS];
	}

	int i = 0;
	int rank = 0;
	int file = 0;
	//Set up pieces
	while(FEN[i] != ' '){
		if(FEN[i] >= 0x30 && FEN[i] <= 0x39){
			for(int j = 0; j < FEN[i] - 0x30; j++){
				board[file + j][rank] = new Piece((file + j) * 100, rank * 100);
			}
			file += FEN[i] - 0x30;
			i++;
			continue;
		}
		switch(FEN[i]){
			case '/':
				file = -1;
				rank++;
				break;
			case 'r':
				board[file][rank] = new Piece(ROOK | BLACK, IMG_LoadTexture(rend, "img/blackRook.png"), 0, file * 100, rank * 100);
				break;
			case 'n':
				board[file][rank] = new Piece(KNIGHT | BLACK, IMG_LoadTexture(rend, "img/blackKnight.png"), 0, file * 100, rank * 100);
				break;
			case 'b':
				board[file][rank] = new Piece(BISHOP | BLACK, IMG_LoadTexture(rend, "img/blackBishop.png"), 0, file * 100, rank * 100);
				break;
			case 'q':
				board[file][rank] = new Piece(QUEEN | BLACK, IMG_LoadTexture(rend, "img/blackQueen.png"), 0, file * 100, rank * 100);
				break;
			case 'k':
				board[file][rank] = new Piece(KING | BLACK, IMG_LoadTexture(rend, "img/blackKing.png"), 0, file * 100, rank * 100);
				break;
			case 'p':
				board[file][rank] = new Piece(PAWN | BLACK, IMG_LoadTexture(rend, "img/blackPawn.png"), 0, file * 100, rank * 100);
				break;
			case 'R':
				board[file][rank] = new Piece(ROOK | WHITE, IMG_LoadTexture(rend, "img/whiteRook.png"), 0, file * 100, rank * 100);
				break;
			case 'N':
				board[file][rank] = new Piece(KNIGHT | WHITE, IMG_LoadTexture(rend, "img/whiteKnight.png"), 0, file * 100, rank * 100);
				break;
			case 'B':
				board[file][rank] = new Piece(BISHOP | WHITE, IMG_LoadTexture(rend, "img/whiteBishop.png"), 0, file * 100, rank * 100);
				break;
			case 'Q':
				board[file][rank] = new Piece(QUEEN | WHITE, IMG_LoadTexture(rend, "img/whiteQueen.png"), 0, file * 100, rank * 100);
				break;
			case 'K':
				board[file][rank] = new Piece(KING | WHITE, IMG_LoadTexture(rend, "img/whiteKing.png"), 0, file * 100, rank * 100);
				break;
			case 'P':
				board[file][rank] = new Piece(PAWN | WHITE, IMG_LoadTexture(rend, "img/whitePawn.png"), 0, file * 100, rank * 100);
				break;
				
		}
		i++;
		file++;
	}

	if(FEN[++i] == 'w'){
		*color = WHITE;
	}

	else{
		*color = BLACK;
	}

	i++;	//Should be index on space after this is run

	if(FEN[++i] == 'K'){
		board[4][7]->statusFlag |= CANCASTLE; 	//king will always be in starting location if it can castle
		board[7][7]->statusFlag |= CANCASTLE; 	//rook will always be in starting location if it can castle
		i++;
	}

	if(FEN[i] == 'Q'){	
		board[4][7]->statusFlag |= CANCASTLE; 	//king will always be in starting location if it can castle
		board[0][7]->statusFlag |= CANCASTLE; 	//rook will always be in starting location if it can castle
		i++;
	}

	if(FEN[i] == 'k'){	
		board[4][0]->statusFlag |= CANCASTLE; 	//king will always be in starting location if it can castle
		board[7][0]->statusFlag |= CANCASTLE; 	//rook will always be in starting location if it can castle
		i++;
	}

	if(FEN[i] == 'q'){	
		board[4][0]->statusFlag |= CANCASTLE; 	//king will always be in starting location if it can castle
		board[0][0]->statusFlag |= CANCASTLE;	//rook will always be in starting location if it can castle
		i++;
	}

	i++;	//Should be index on space after this is run

	return board;
}

void freeTextures(Piece ***board){
	for(int i = 0; i < COLLUMNS; i++){
		for(int j = 0; j < ROWS; j++){
			if(board[i][j]->pieceImg != NULL){
				SDL_DestroyTexture(board[i][j]->pieceImg);
			}
		}
	}
}

void freeBoard(Piece ***board){
	for(int i = 0; i < COLLUMNS; i++){
		for(int j = 0; j < ROWS; j++){
			delete board[i][j];
		}
	}
	for(int i = 0; i < ROWS; i++){
		delete[] board[i];
	}
	delete[] board;
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

	Piece **promoPieces = new Piece*[4];

	promoPieces[0] = new Piece(QUEEN | color, (color == WHITE) ? IMG_LoadTexture(rend, "img/whiteQueen.png") : IMG_LoadTexture(rend, "img/blackQueen.png"), 0, 0, 0);
	promoPieces[1] = new Piece(ROOK | color, (color == WHITE) ? IMG_LoadTexture(rend, "img/whiteRook.png") : IMG_LoadTexture(rend, "img/blackRook.png"), 0, 100, 0);
	promoPieces[2] = new Piece(BISHOP | color, (color == WHITE) ? IMG_LoadTexture(rend, "img/whiteBishop.png") : IMG_LoadTexture(rend, "img/blackBishop.png"), 0, 200, 0);
	promoPieces[3] = new Piece(KNIGHT | color, (color == WHITE) ? IMG_LoadTexture(rend, "img/whiteKnight.png") : IMG_LoadTexture(rend, "img/blackKnight.png"), 0, 300, 0);

	for(int i = 0; i < 4; i++){
		SDL_RenderCopy(rend, promoPieces[i]->pieceImg, NULL, &(promoPieces[i]->rect));
	}

	SDL_RenderPresent(rend);

	while(1){
		while(SDL_PollEvent(&event)){
			switch(event.type){
				case SDL_MOUSEBUTTONDOWN:
					for(int i = 0; i < 4; i++){
						SDL_DestroyTexture(promoPieces[i]->pieceImg);
						delete promoPieces[i];
					}
					delete [] promoPieces;
					SDL_DestroyTexture(backgroundTexture);
					delete pixels;
					SDL_DestroyRenderer(rend);
					SDL_DestroyWindow(wind);
					switch(event.button.x / 100){
						case 0:
							return PAWNPROMOQUEENFLAG;
						case 1:
							return PAWNPROMOROOKFLAG;
						case 2:
							return PAWNPROMOBISHOPFLAG;
						case 3:
							return PAWNPROMOKNIGHTFLAG; 
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

	Piece *pieceSelected = NULL;

	int color;

	int pieceSelectedState = 0;	//0 = no piece clicked, 1 = piece clicked, 2 = move selected

	int pieceSelectedX = 0;

	int pieceSelectedY = 0;

	Piece ***board = fenToBoard("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq -", rend, &color);
	
	while(running){
		while(SDL_PollEvent(&event)){
			switch(event.type){
				case SDL_QUIT:
					running = 0;
					break;
				case SDL_MOUSEBUTTONDOWN:
					if(!pieceSelectedState){
						pieceSelectedState = 1;
						pieceSelectedX = event.button.x / 100;
						pieceSelectedY = event.button.y / 100;
						pieceSelected = board[pieceSelectedX][pieceSelectedY]; 
					}
					else{
						pieceSelectedState = 2;
						pieceSelectedX = event.button.x / 100;
						pieceSelectedY = event.button.y / 100;
					}
			}
		}

		int isMoveLegal = 0;

		if(pieceSelectedState == 2){
			uint8_t promoChoice = PAWNPROMOQUEENFLAG;
			if((pieceSelected->pieceID & 0b00111) == PAWN && (pieceSelectedY == 7 || pieceSelectedY == 0)){
				promoChoice = promoWindow(color);
			}
			isMoveLegal = pieceSelected->move(board, board[pieceSelectedX][pieceSelectedY], color, promoChoice, rend);
			pieceSelectedState = 0;
		}

		for(int i = 0; i < WIDTH; i++){
			for(int j = 0; j < HEIGHT; j++){
				//unsigned char val = frame->tiles[i / 8][j / 8].pixels[i % 8][j % 8];
				//
				if((((j + 1) / 100) % 2 != 0 && ((i + 1) / 100) % 2 != 0) || (((j + 1) / 100) % 2 == 0 && ((i + 1) / 100) % 2 == 0)){
					pixels[j + i * WIDTH] = 240 << 24 | 239 << 16 | 194 << 8 | 255;	
				}
				else{
					pixels[j + i * WIDTH] = 238 << 24 | 115 << 16 | 118 << 8 | 255;
				}

			}
		}

		
		if(pieceSelectedState == 1 && board[pieceSelectedX][pieceSelectedY]->pieceID != 0){
			highlightSelectedPiece(pixels, pieceSelectedX * 100, pieceSelectedY * 100);	
		}
		else{
			pieceSelectedState = 0;
		}

		SDL_UpdateTexture(boardTexture, NULL, pixels, sizeof(Uint32) * WIDTH);
		SDL_RenderCopy(rend, boardTexture, NULL, NULL);
		
		for(int i = 0; i < ROWS; i++){
			for(int j = 0; j < COLLUMNS; j++){
				if(board[i][j]->pieceID != 0){
					SDL_RenderCopy(rend, board[i][j]->pieceImg, NULL, &(board[i][j]->rect));
				}
			}
		}
		/* Draw to window and loop */
		//SDL_RenderClear(rend);
		SDL_RenderPresent(rend);
		SDL_Delay(1000/FPS);
		
		if(isMoveLegal){
			color = (color == WHITE) ? BLACK : WHITE;
		}

	}

	/* Release resources */
	SDL_DestroyTexture(boardTexture);
	freeTextures(board);
	SDL_DestroyRenderer(rend);
	SDL_DestroyWindow(wind);
	freeBoard(board);
	delete[] pixels;
	return 0;
}

int main(){ 
	SDL_Window *wind = initDisplay(WIDTH, HEIGHT, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 0);

	SDL_Renderer *rend = initRender(wind);

	displayLoop(wind, rend);
	return 0;
}
