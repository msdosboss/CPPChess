#include "openBook.hpp"

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
    for(int i = 0; i < 8; i++){
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
    for(int i = 0; i < 8; i++){
        zobristEnPassant[i] = keys["zobristEnPassant"][i];
    }
    zobristSideToMove = keys["zobristSideToMove"];
}

uint64_t boardStateHash(BoardState& boardState){
    uint64_t zobristHash = 0;
    for(int color = WHITE; color <= BLACK; color++){
        for(int pieceType = PAWN; pieceType <= KING; pieceType++){
            uint64_t bb = boardState.pieces[color][pieceType];
            while(bb != 0){
                int squareIndex = __builtin_ctzll(bb);
                zobristHash ^= zobristTable[pieceType + (color * 6)][squareIndex];

                clearBit(bb, squareIndex);
            }
        }
    }
    //No key is applied if it white to move
    if(boardState.sideToMove == BLACK){
        zobristHash ^= zobristSideToMove;
    }
    if(boardState.castlingRights.queenSideCastleBlack){
        zobristHash ^= zobristCastle[BLACKQUEENCASTLE];
    }
    if(boardState.castlingRights.kingSideCastleBlack){
        zobristHash ^= zobristCastle[BLACKKINGCASTLE];
    }
    if(boardState.castlingRights.queenSideCastleWhite){
        zobristHash ^= zobristCastle[WHITEQUEENCASTLE];
    }
    if(boardState.castlingRights.kingSideCastleWhite){
        zobristHash ^= zobristCastle[WHITEKINGCASTLE];
    }
    if(boardState.enPassantSquare != -1){
        int enPassantFile = boardState.enPassantSquare % 8;
        zobristHash ^= zobristEnPassant[enPassantFile];
    }

    return zobristHash;
}

void zobristFlags(BoardState& boardState){
    if(boardState.castlingRights.queenSideCastleBlack){
        boardState.zobristHash ^= zobristCastle[BLACKQUEENCASTLE];
    }
    if(boardState.castlingRights.kingSideCastleBlack){
        boardState.zobristHash ^= zobristCastle[BLACKKINGCASTLE];
    }
    if(boardState.castlingRights.queenSideCastleWhite){
        boardState.zobristHash ^= zobristCastle[WHITEQUEENCASTLE];
    }
    if(boardState.castlingRights.kingSideCastleWhite){
        boardState.zobristHash ^= zobristCastle[WHITEKINGCASTLE];
    }
    if(boardState.enPassantSquare != -1){
        int enPassantFile = boardState.enPassantSquare % 8;
        boardState.zobristHash ^= zobristEnPassant[enPassantFile];
    }
}
