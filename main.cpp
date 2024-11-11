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

Piece ***fenToBoard(const char *FEN, SDL_Renderer *rend){
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
				board[file + j][rank] = new Piece;
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
	return board;
}

Piece ***initBoard(SDL_Renderer *rend){
	Piece ***board = new Piece**[ROWS];
	for(int i = 0; i < ROWS; i++){
		board[i] = new Piece*[COLLUMNS];
	}

	//White Pieces
	board[0][0] = new Piece(ROOK | WHITE, IMG_LoadTexture(rend, "img/whiteRook.png"), 0, 0, 700);
	board[1][0] = new Piece(KNIGHT | WHITE, IMG_LoadTexture(rend, "img/whiteKnight.png"), 0, 100, 700);
	board[2][0] = new Piece(BISHOP | WHITE, IMG_LoadTexture(rend, "img/whiteBishop.png"), 0, 200, 700);
	board[3][0] = new Piece(QUEEN | WHITE, IMG_LoadTexture(rend, "img/whiteQueen.png"), 0, 300, 700);
	board[4][0] = new Piece(KING | WHITE, IMG_LoadTexture(rend, "img/whiteKing.png"), 0, 400, 700);
	board[5][0] = new Piece(BISHOP | WHITE, IMG_LoadTexture(rend, "img/whiteBishop.png"), 0, 500, 700);
	board[6][0] = new Piece(KNIGHT | WHITE, IMG_LoadTexture(rend, "img/whiteKnight.png"), 0, 600, 700);
	board[7][0] = new Piece(ROOK | WHITE, IMG_LoadTexture(rend, "img/whiteRook.png"), 0, 700, 700);

	//White pawns
	for(int i = 0; i < ROWS; i++){
		board[i][1] = new Piece(PAWN | WHITE, IMG_LoadTexture(rend, "img/whitePawn.png"), 0, i * 100, 600);
	}
	//middle of the board (empty)
	for(int i = 2; i < 6; i++){
		for(int j = 0; j < ROWS; j++){
			board[j][i] = new Piece;	
		}
	}
	//Black Pawns
	for(int i = 0; i < ROWS; i++){
		board[i][6] = new Piece(PAWN | BLACK, IMG_LoadTexture(rend, "img/blackPawn.png"), 0, i * 100, 100);
	}
	//Black Pieces
	board[0][7] = new Piece(ROOK | BLACK, IMG_LoadTexture(rend, "img/blackRook.png"), 0, 0, 0);
	board[1][7] = new Piece(KNIGHT | BLACK, IMG_LoadTexture(rend, "img/blackKnight.png"), 0, 100, 0);
	board[2][7] = new Piece(BISHOP | BLACK, IMG_LoadTexture(rend, "img/blackBishop.png"), 0, 200, 0);
	board[3][7] = new Piece(QUEEN | BLACK, IMG_LoadTexture(rend, "img/blackQueen.png"), 0, 300, 0);
	board[4][7] = new Piece(KING | BLACK, IMG_LoadTexture(rend, "img/blackKing.png"), 0, 400, 0);
	board[5][7] = new Piece(BISHOP | BLACK, IMG_LoadTexture(rend, "img/blackBishop.png"), 0, 500, 0);
	board[6][7] = new Piece(KNIGHT | BLACK, IMG_LoadTexture(rend, "img/blackKnight.png"), 0, 600, 0);
	board[7][7] = new Piece(ROOK | BLACK, IMG_LoadTexture(rend, "img/blackRook.png"), 0, 700, 0);

	return board;
}

int displayLoop(SDL_Window *wind, SDL_Renderer *rend){

	SDL_Event event;

	SDL_Texture *boardTexture = SDL_CreateTexture(rend, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, WIDTH, HEIGHT);

	int running = 1;

	Uint32 *pixels = new Uint32[WIDTH * HEIGHT];

	//Piece ***board = initBoard(rend);

	Piece ***board = fenToBoard("r2q1rk1/1bp1bppp/p1np1n2/1p2p3/3PP3/2P2N1P/PPB2PP1/RNBQR1K1 ", rend);

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
					pixels[j + i * WIDTH] = 240 << 24 | 239 << 16 | 194 << 8 | 255;	
				}
				else{
					pixels[j + i * WIDTH] = 238 << 24 | 115 << 16 | 118 << 8 | 255;
				}

			}
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
