#ifndef OPENBOOK_H
#define OPENBOOK_H

#include <cstdint>
#include <random>
#include "nlohmann/json.hpp"
#include <fstream>
#include <iostream>
#include <cassert>

#define WHITEKINGCASTLE  0
#define WHITEQUEENCASTLE 1
#define BLACKKINGCASTLE  2
#define BLACKQUEENCASTLE 3

void initZobristTable(const std::string fileName);
void createZobristKeys(const std::string fileName);
#endif
