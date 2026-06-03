#include "openBook.hpp"

uint64_t zobristTable[12][64];
uint64_t zobristCastle[4];
uint64_t zobristSideToMove;
uint64_t zobristEnPassant[64];

void createZobristKeys(const std::string fileName){
    std::random_device rd;
    std::mt19937_64 gen(rd());
    std::uniform_int_distribution<std::uint64_t> distrib(
        0,
        std::numeric_limits<std::uint64_t>::max()
    );
    zobristSideToMove = distrib(gen);
    for(int i = 0; i < 12; i++){
        for(int j = 0; j < 64; j++){
            zobristTable[i][j] = distrib(gen);
        }
    }
    for(int i = 0; i < 4; i++){
        zobristCastle[i] = distrib(gen);
    }
    for(int i = 0; i < 64; i++){
        zobristEnPassant[i] = distrib(gen);
    }

    nlohmann::json keys;
    keys["zobristTable"] = zobristTable;
    keys["zobristCastle"] = zobristCastle;
    keys["zobristSideToMove"] = zobristSideToMove;
    keys["zobristEnPassant"] = zobristEnPassant;

    std::ofstream json(fileName);
    json << keys.dump(4);
}

void initZobristTable(const std::string fileName){
    std::ifstream json(fileName);
    
    if(!json){
        std::cerr << "failed to open json file: " << fileName << '\n';
        return;
    }
    nlohmann::json keys = nlohmann::json::parse(json);

    if(keys.empty()){
        std::cerr << "failed to parse json file: " << fileName << '\n';
        return;
    }

    for(int i = 0; i < 12; i++){
        for(int j = 0; j < 64; j++){
            zobristTable[i][j] = keys["zobristTable"][i][j];
        }
    }

    for(int i = 0; i < 4; i++){
        zobristCastle[i] = keys["zobristCastle"][i];
    }
    for(int i = 0; i < 64; i++){
        zobristEnPassant[i] = keys["zobristEnPassant"][i];
    }
    zobristSideToMove = keys["zobristSideToMove"];
}
