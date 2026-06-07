#ifndef TRANSPOSTIONTABLE_H
#define TRANSPOSTIONTABLE_H

#include "physics.hpp"
#include <vector>
#include <algorithm>

struct TTEntry {
    uint64_t key;
    int score;
    int depth;
    uint8_t flag;
    Move bestMove;
};

struct TTBucket {
    TTEntry deepEntry;
    TTEntry newEntry;
};

class TranspositionTable{
    private:
        std::vector<TTBucket> table;
        int numEntries;
    public:
        void resize(size_t megabytes);
        void clear();
        void store(uint64_t hash, int depth, int score, uint8_t flag, Move bestMove);
        bool probe(uint64_t hash, TTEntry& returnEntry);
};

inline TranspositionTable TT;

#endif
