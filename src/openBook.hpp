#ifndef OPENBOOK_H
#define OPENBOOK_H

#include "../lib/nlohmann/json.hpp"
#include "physics.hpp"
#include <cstdint>
#include <random>
#include <fstream>
#include <iostream>
#include <cassert>
#include <vector>
#include <unordered_map>

inline uint64_t zobristTable[12][64];
inline uint64_t zobristCastle[4];
inline uint64_t zobristSideToMove;
inline uint64_t zobristEnPassant[8];

struct BookMove{
    Move move;
    int weight;
};

inline std::unordered_map<uint64_t, std::vector<BookMove>> openBook;


#define WHITEKINGCASTLE  0
#define WHITEQUEENCASTLE 1
#define BLACKKINGCASTLE  2
#define BLACKQUEENCASTLE 3


void loadOpeningBook(const std::string& fileName);
bool getBookMove(uint64_t zobristHash, Move& openMove);
void initZobristTable(const std::string fileName);
void createZobristKeys(const std::string fileName);
uint64_t boardStateHash(BoardState& boardState);
void zobristFlags(BoardState& boardState);

#endif
