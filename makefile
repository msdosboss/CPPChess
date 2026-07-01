CXX = g++
CXXFLAGS = -std=c++17 -Wall -O3
SDL_FLAGS = `sdl2-config --cflags --libs` -lSDL2_image

# 'all' is the default target. It tells Make to build both executables.
all: UCIClient engine createKeys engineClient matchManager

createKeys: objects/createKeys.o objects/openBook.o
	$(CXX) $(CXXFLAGS) -o build/createKeys objects/createKeys.o objects/openBook.o

debug: debugMain debugEngine

debugMain: src/UCIClient.cpp src/physics.cpp src/engineProcess.cpp src/openBook.cpp
	$(CXX) $(CXXFLAGS) -g -o build/main src/UCIClient.cpp src/physics.cpp src/engineProcess.cpp src/openBook.cpp $(SDL_FLAGS)

debugEngine: src/engine.cpp src/search.cpp src/evaluate.cpp src/physics.cpp src/openBook.cpp src/transpositionTable.cpp
	$(CXX) $(CXXFlags) -g -o build/engine src/engine.cpp src/search.cpp src/evaluate.cpp src/physics.cpp src/openBook.cpp src/transpositionTable.cpp


# Compile the GUI
UCIClient: objects/UCIClient.o objects/physics.o objects/engineProcess.o objects/openBook.o objects/gui.o
	$(CXX) $(CXXFLAGS) -o build/UCIClient objects/UCIClient.o objects/physics.o objects/engineProcess.o objects/openBook.o objects/gui.o $(SDL_FLAGS)

matchManager: objects/matchManager.o objects/physics.o objects/openBook.o objects/gui.o
	$(CXX) $(CXXFLAGS) $(SDL_FLAGS) -o build/matchManager objects/matchManager.o objects/physics.o objects/openBook.o objects/gui.o

objects/matchManager.o: src/matchManager.cpp
	$(CXX) $(CXXFLAGS) $(SDL_FLAGS) -c -o objects/matchManager.o src/matchManager.cpp

engineClient: objects/netClient.o objects/engineProcess.o objects/engineClient.o objects/physics.o objects/openBook.o
	$(CXX) $(CXXFlags) $(SDL_FLAGS) -o build/engineClient objects/netClient.o objects/engineProcess.o objects/engineClient.o objects/physics.o objects/openBook.o

objects/engineClient.o: src/engineClient.cpp
	$(CXX) $(CXXFlags) $(SDL_FLAGS) -c -o objects/engineClient.o src/engineClient.cpp

objects/netClient.o: src/netClient.cpp
	$(CXX) $(CXXFlags) $(SDL_FLAGS) -c -o objects/netClient.o src/netClient.cpp

objects/createKeys.o: src/createKeys.cpp
	$(CXX) $(CXXFLAGS) -c -o objects/createKeys.o src/createKeys.cpp

objects/UCIClient.o: src/UCIClient.cpp 
	$(CXX) $(CXXFLAGS) -c -o objects/UCIClient.o src/UCIClient.cpp $(SDL_FLAGS)

objects/gui.o: src/gui.cpp
	$(CXX) $(CXXFlags) $(SDL_FLAGS) -c -o objects/gui.o src/gui.cpp

# Compile the standalone engine
engine: objects/engine.o objects/evaluate.o objects/search.o objects/physics.o objects/openBook.o objects/transpositionTable.o
	$(CXX) $(CXXFLAGS) -o build/engine objects/engine.o objects/physics.o objects/evaluate.o objects/search.o objects/openBook.o objects/transpositionTable.o

objects/engine.o: src/engine.cpp
	$(CXX) $(CXXFLAGS) -c -o objects/engine.o src/engine.cpp  

objects/physics.o: src/physics.cpp
	$(CXX) $(CXXFLAGS) -c -o objects/physics.o src/physics.cpp

objects/engineProcess.o: src/engineProcess.cpp
	$(CXX) $(CXXFLAGS) $(SDL_FLAGS) -c -o objects/engineProcess.o src/engineProcess.cpp

objects/search.o: src/search.cpp
	$(CXX) $(CXXFLAGS) -c -o objects/search.o src/search.cpp

objects/evaluate.o: src/evaluate.cpp
	$(CXX) $(CXXFLAGS) -c -o objects/evaluate.o src/evaluate.cpp

objects/openBook.o: src/openBook.cpp
	$(CXX) $(CXXFLAGS) -c -o objects/openBook.o src/openBook.cpp	

objects/transpositionTable.o: src/transpositionTable.cpp
	$(CXX) $(CXXFLAGS) -c -o objects/transpositionTable.o src/transpositionTable.cpp	

# Clean up both executables
clean:
	rm -f objects/*.o
	rm -f build/main build/engine build/createKeys build/netClient build/matchManager build/engineClient build/UCIClient
