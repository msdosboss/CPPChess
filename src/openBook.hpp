#ifndef OPENBOOK_H
#define OPENBOOK_H

#include "../lib/nlohmann/json.hpp"
#include "physics.hpp"
#include <cstdint>
#include <random>
#include <fstream>
#include <iostream>
#include <cassert>

inline uint64_t zobristTable[12][64];
inline uint64_t zobristCastle[4];
inline uint64_t zobristSideToMove;
inline uint64_t zobristEnPassant[8];


#define WHITEKINGCASTLE  0
#define WHITEQUEENCASTLE 1
#define BLACKKINGCASTLE  2
#define BLACKQUEENCASTLE 3

void initZobristTable(const std::string fileName);
void createZobristKeys(const std::string fileName);
uint64_t boardStateHash(BoardState& boardState);
void zobristFlags(BoardState& boardState);

#endif
