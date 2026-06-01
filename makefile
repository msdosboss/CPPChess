CXX = g++
CXXFLAGS = -std=c++17 -Wall -O3
SDL_FLAGS = `sdl2-config --cflags --libs` -lSDL2_image

# 'all' is the default target. It tells Make to build both executables.
all: main engine

# Compile the GUI
main: main.cpp physics.cpp engineProcess.cpp
	$(CXX) $(CXXFLAGS) -o main main.cpp physics.cpp engineProcess.cpp evaluate.cpp $(SDL_FLAGS)

# Compile the standalone engine
engine: engine.cpp evaluate.cpp search.cpp physics.cpp
	$(CXX) $(CXXFLAGS) -o engine engine.cpp physics.cpp evaluate.cpp search.cpp

# Clean up both executables
clean:
	rm -f main engine
