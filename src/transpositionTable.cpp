#include "transpositionTable.hpp"

void TranspositionTable::resize(size_t megaBytes){
    numEntries = (megaBytes * 1024 * 1024) / sizeof(TTBucket);
    //Fill table with empty entries
    table.assign(numEntries, {{0}, {0}});
}

void TranspositionTable::clear(){
    std::fill(table.begin(), table.end(), TTBucket{{0}, {0}});
}

void TranspositionTable::store(uint64_t hash, int depth, int score, uint8_t flag, Move bestMove){
    size_t index = hash % numEntries;

    if(table[index].deepEntry.depth < depth){
        table[index].deepEntry.score = score;
        table[index].deepEntry.depth = depth;
        table[index].deepEntry.flag = flag;
        table[index].deepEntry.bestMove = bestMove;
        table[index].deepEntry.key = hash ^ table[index].deepEntry.data;
    }
    table[index].newEntry.score = score;
    table[index].newEntry.depth = depth;
    table[index].newEntry.flag = flag;
    table[index].newEntry.bestMove = bestMove;
    table[index].newEntry.key = hash ^ table[index].newEntry.data;

}

bool TranspositionTable::probe(uint64_t hash, TTEntry& returnEntry){
    size_t index = hash % numEntries;
    if((table[index].deepEntry.key ^ table[index].deepEntry.data) == hash){
        returnEntry = table[index].deepEntry;
        return true;
    }
    if((table[index].newEntry.key ^ table[index].newEntry.data) == hash){
        returnEntry = table[index].newEntry;
        return true;
    }
    return false;
}
