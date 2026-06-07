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
        table[index].deepEntry = {hash, score, depth, flag, {bestMove.raw}};
    }
    
    table[index].newEntry = {hash, score, depth, flag, {bestMove.raw}};
}

bool TranspositionTable::probe(uint64_t hash, TTEntry& returnEntry){
    size_t index = hash % numEntries;
    if(table[index].deepEntry.key == hash){
        returnEntry = table[index].deepEntry;
        return true;
    }
    if(table[index].newEntry.key == hash){
        returnEntry = table[index].newEntry;
        return true;
    }
    return false;
}
