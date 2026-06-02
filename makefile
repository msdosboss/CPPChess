CXX = g++
CXXFLAGS = -std=c++17 -Wall -O3
SDL_FLAGS = `sdl2-config --cflags --libs` -lSDL2_image

# 'all' is the default target. It tells Make to build both executables.
all: main engine

debug: debugMain debugEngine

debugMain: main.cpp physics.cpp engineProcess.cpp
	$(CXX) $(CXXFLAGS) -g -o main main.cpp physics.cpp engineProcess.cpp $(SDL_FLAGS)

debugEngine: engine.cpp search.cpp evaluate.cpp physics.cpp
	$(CXX) $(CXXFlags) -g -o engine engine.cpp search.cpp evaluate.cpp physics.cpp


# Compile the GUI
main: main.o physics.o engineProcess.o 
	$(CXX) $(CXXFLAGS) -o main main.o physics.o engineProcess.o $(SDL_FLAGS)

main.o: main.cpp 
	$(CXX) $(CXXFLAGS) -c -o main.o main.cpp $(SDL_FLAGS)

# Compile the standalone engine
engine: engine.cpp evaluate.o search.o physics.o
	$(CXX) $(CXXFLAGS) -o engine engine.cpp physics.o evaluate.o search.o

engine.o: engine.o
	$(CXX) $(CXXFLAGS) -c -o engine.o engine.cpp  

physics.o: physics.cpp
	$(CXX) $(CXXFLAGS) -c -o physics.o physics.cpp

engineProcess.o: engineProcess.cpp
	$(CXX) $(CXXFLAGS) $(SDL_FLAGS) -c -o engineProcess.o engineProcess.cpp

search.o: search.cpp
	$(CXX) $(CXXFLAGS) -c -o search.o search.cpp

evaluate.o: evaluate.cpp
	$(CXX) $(CXXFLAGS) -c -o evaluate.o evaluate.cpp

# Clean up both executables
clean:
	rm -f *.o
	rm -f main engine
