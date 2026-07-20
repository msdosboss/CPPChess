#ifndef __MATCH_MANAGER_HPP__
#define __MATCH_MANAGER_HPP__

#include "physics.hpp"
#include "gui.hpp"
#include "netUtils.hpp"
#include "openBook.hpp"
#include <thread>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sstream>
#include <string>
#include <cstring>
#include <cmath>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <unistd.h>
#include <fcntl.h>
#include <cassert>
#include <cstdlib>
#include <chrono>
#include <unordered_map>

struct GameState {
    std::atomic<bool> gameOver;
    std::atomic<bool> whiteReady;
    std::atomic<bool> blackReady;
    std::atomic<bool> timeUp;
    std::atomic<int> turnState;
    std::atomic<std::time_t> blackTime; //In milliseconds
    std::atomic<std::time_t> whiteTime;
    std::atomic<std::time_t> blackTotalTime; //In milliseconds
    std::atomic<std::time_t> whiteTotalTime;
    std::atomic<int> baseLineEngineColor;
    std::atomic<int> testEngineColor;
    std::mutex threadSyncMutex;
    std::condition_variable mutexCondition;
    BoardState state;
};

#define UNFINISHED 3
#define DRAW 2
#define ACCEPT 1
#define CONTINUE 0
#define REJECT -1

struct GameHistory {
    uint8_t winner; //0 for white 1 for black 2 for draw
    unsigned int moveIndex;
    std::string whiteName;
    std::string blackName;
    std::string startFen;
    std::string moveFens[512];
    struct UndoState undoStates[512];
    Move moves[512]; //Sensible default for a max-length game
	std::string fenDBFilePath;
};

struct SPRTInformation{
    int eloDiff;
    double falsePositive;
    double falseNegative;
};

void engineThread(
    struct GameState& gameState,
    struct GameHistory& gameHistory,
    std::atomic<int>& color,
    std::string& UCIResponse,
    bool& responseReady
);

void matchManagerThread(
    struct GameState& gameState,
    struct GameHistory& gameHistory,
    struct SPRTInformation SPRTInfo,
	int gamesToPlay,
    bool& responseReady,
    std::string& UCIResponse
);

void CLIThread(
    std::atomic<bool>& gameOver,
    std::atomic<bool>& timeUp,
    unsigned int& gamesToPlay,
    struct GameHistory& gameHistory,
    std::mutex& m,
    std::condition_variable& cv
);

void gameHistoryToFile(struct GameHistory& history, const std::string filename);
double SPRTVariance(int totalWins, int totalLoses, int totalDraws);
double SPRTExpectedScore(int eloDiff);
double SPRTLLR(int totalWins, int totalLoses, int totalDraws, int eloDiff);
int SPRTTest(
    double falsePositive,
    double falseNegative,
    int totalWins,
    int totalLoses,
    int toatlDraws,
    int eloDiff    
);

bool drawByInsufficientMaterial(struct BoardState& state);
bool drawByRepetition(struct GameHistory& gameHistory);
#define LISTEN_PORT_WHITE 3001
#define LISTEN_PORT_BLACK 3002
const int ENGINE_LISTEN_PORT[2] = {LISTEN_PORT_WHITE, LISTEN_PORT_BLACK};

#endif // #ifndef __MATCH_MANAGER_HPP__
