//g++ -o main main.cpp `sdl2-config --cflags --libs` -lSDL2_image
#include <iostream>
#include <SDL.h>
#include <SDL_image.h>

#define WIDTH 800
#define HEIGHT 800
#define COLLUMNS 8
#define ROWS 8
#define FPS 60
#define SIZE 100

#define PAWN   0b00000001
#define KNIGHT 0b00000010
#define BISHOP 0b00000011
#define ROOK   0b00000100
#define QUEEN  0b00000101
#define KING   0b00000111

#define WHITE  0b00001000
#define BLACK  0b00010000

class Piece {
	public:
		uint8_t pieceID;
		SDL_Texture *pieceImg;
		uint8_t statusFlag;	//will be used for things like castle and en pasente
		SDL_Rect rect;
	
		Piece(){
			pieceID = 0;
			pieceImg = NULL;
			statusFlag = 0;
		}


		Piece(uint8_t pieceIDCon, SDL_Texture *pieceImgCon, uint8_t statusFlagCon, int xPos, int yPos){
			pieceID = pieceIDCon;
			pieceImg = pieceImgCon;
			statusFlag = statusFlagCon;
			rect = {(int) xPos, (int) yPos, SIZE, SIZE};
		}
};

SDL_Window *initDisplay(){
	/* Initializes the timer, audio, video, joystick,
	haptic, gamecontroller and events subsystems */
	if(SDL_Init(SDL_INIT_EVERYTHING) != 0){
		printf("Error initializing SDL: %s\n", SDL_GetError());
		return 0;
	}
	/* Create a window */
	SDL_Window* wind = SDL_CreateWindow("Hello Platformer!",
				      SDL_WINDOWPOS_CENTERED,
				      SDL_WINDOWPOS_CENTERED,
				      WIDTH, HEIGHT, 0);
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

Piece ***initBoard(SDL_Renderer *rend){
	Piece ***board = new Piece**[ROWS];
	for(int i = 0; i < ROWS; i++){
		board[i] = new Piece*[COLLUMNS];
	}

	//White Pieces (will do later)
	for(int i = 0; i < ROWS; i++){
		board[i][0] = new Piece;
	}

	//White pawns
	for(int i = 0; i < ROWS; i++){
		board[i][1] = new Piece(PAWN | WHITE, IMG_LoadTexture(rend, "img/whitePawn.png"), 0, i, 1);
	}
	//middle of the board (empty)
	for(int i = 2; i < 6; i++){
		for(int j = 0; j < ROWS; j++){
			board[j][i] = new Piece;	
		}
	}
	//Black Pawns
	for(int i = 0; i < ROWS; i++){
		board[i][6] = new Piece(PAWN | BLACK, IMG_LoadTexture(rend, "img/whitePawn.png"), 0, i, 1);
	}
	//Black Pieces (will do later)
	for(int i = 0; i < ROWS; i++){
		board[i][7] = new Piece;
	}

	return board;
}

int displayLoop(SDL_Window *wind, SDL_Renderer *rend){

	SDL_Event event;

	SDL_Texture *boardTexture = SDL_CreateTexture(rend, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, WIDTH, HEIGHT);

	int running = 1;

	Uint32 *pixels = new Uint32[WIDTH * HEIGHT];

	Piece ***board = initBoard(rend);

	while(running){
		while(SDL_PollEvent(&event)){
			switch(event.type){
				case SDL_QUIT:
					running = 0;
					break;
			}
		}
		for(int i = 0; i < WIDTH; i++){
			for(int j = 0; j < HEIGHT; j++){
				//unsigned char val = frame->tiles[i / 8][j / 8].pixels[i % 8][j % 8];
				//
				if((((j + 1) / 100) % 2 != 0 && ((i + 1) / 100) % 2 != 0) || (((j + 1) / 100) % 2 == 0 && ((i + 1) / 100) % 2 == 0)){
					pixels[j + i * WIDTH] = 255 << 24 | 255 << 16 | 255 << 8 | 255;
				}
				else{
					pixels[j + i * WIDTH] = 0 << 24 | 0 << 16 | 0 << 8 | 255;	
				}

			}
		}
		SDL_UpdateTexture(boardTexture, NULL, pixels, sizeof(Uint32) * WIDTH);
		SDL_RenderCopy(rend, boardTexture, NULL, NULL);

		for(int i = 0; i < ROWS; i++){
			for(int j = 0; j < ROWS; j++){
				if(board[i][j]->pieceID != 0){
					SDL_RenderCopy(rend, board[i][j]->pieceImg, NULL, &(board[i][j]->rect));
				}
			}
		}

		/* Draw to window and loop */
		//SDL_RenderClear(rend);
		SDL_RenderPresent(rend);
		SDL_Delay(1000/FPS);

	}

	delete[] pixels;
	return 0;
}

int main(){
	SDL_Window *wind = initDisplay();

	SDL_Renderer *rend = initRender(wind);

	displayLoop(wind, rend);
	return 0;
}
