all: main.cpp
	g++ -o main main.cpp physics.cpp `sdl2-config --cflags --libs` -lSDL2_image

clean:
	rm -f main
