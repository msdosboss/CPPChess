#include "engine.hpp"

int main(){
    std::thread searchThread;
    SearchInfo searchInfo;
    searchInfo.timesUp = false;
    BoardState boardState;
    fenToBoardState("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", boardState);
    std::string line;
    srand(time(NULL));
    generateKingAttacks();
    generateKnightAttacks();
    initZobristTable("data/zobristKeys.json");
    loadOpeningBook("data/openBook.json");
    TT.resize(256);

    while(std::getline(std::cin, line)){
        if(line == "quit"){
            searchInfo.timesUp = true;
            if(searchThread.joinable()){
                searchThread.join();
            }
            break;
        }
        else if(line == "uci"){
            std::cout << "id name Graggle Chess" << std::endl;
            std::cout << "id author Jacob Todd" << std::endl;
            std::cout << "uciok" << std::endl;
        }
        else if(line == "isready"){
            std::cout << "readyok" << std::endl;
        }
        //new board position handled in position command
        else if(line == "ucinewgame"){
            TT.clear();
        }
        else if(line.find("go") != std::string::npos){
            if(searchThread.joinable()){
                searchThread.join();
            }
            int depth = 25;
            int durationSeconds = 0;
            int wTimeSeconds = -1;
            int bTimeSeconds = -1;
            std::istringstream ss(line);
            std::string token;

            while(ss >> token){
                if(token == "wtime"){
                    ss >> token;
                    wTimeSeconds = std::stoi(token) / 1000;
                }
                else if(token == "btime"){
                    ss >> token;
                    bTimeSeconds = std::stoi(token) / 1000;
                }
                else if(token == "depth"){
                    ss >> token;
                    depth = std::stoi(token);
                }
            }
            //defualt to 30 seconds
            if(wTimeSeconds == -1 || bTimeSeconds == -1){
                durationSeconds = 30;
            }
            else{
                if(boardState.sideToMove == WHITE){
                    durationSeconds = std::max(1, wTimeSeconds / 20);
                }
                else{
                    durationSeconds = std::max(1, bTimeSeconds / 20);
                }
            }
            searchInfo.timesUp = false;
            searchThread = std::thread(runSearchWrapper, boardState, depth, std::chrono::seconds(durationSeconds), std::ref(searchInfo));

        }
        else if(line.find("position") == 0){
            std::istringstream ss(line);
            std::string token;

            //Consume the word position
            ss >> token;
            //Consume the word startpos or fen
            ss >> token;
            if(token == "startpos"){
                fenToBoardState("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", boardState);
                //token will be nothing or moves
                ss >> token;
            }
            else if(token == "fen"){
                std::string fen = "";
                
                //Going through each of the tokens in the fen
                //it has 6 spaces normally
                while(ss >> token && token != "moves"){
                    fen += token + " ";
                }
                //Removing trailing space
                if(!fen.empty()){
                    fen.pop_back();
                }
                fenToBoardState(fen, boardState);
            }
            if(token == "moves"){
                while(ss >> token){
                    Move move = strMoveToMove(token, boardState);
                    UndoState undo;
                    if(move.raw != 0){
                        makeMove(boardState, move, undo);
                    }
                } 
            }
        }
        else if(line == "stop"){
            searchInfo.timesUp = true;
        }
        else{
            std::cout << "Failed to parse: " << line << std::endl;
        }
        
    }

    searchInfo.timesUp = true;
    if(searchThread.joinable()){
        searchThread.join();
    }
    return 0;
}


void runSearchWrapper(BoardState boardState, int maxDepth, std::chrono::seconds duration, SearchInfo& searchInfo){
    int finalEval = 0;

    Move selectedMove = searchBestMoveIt(boardState, maxDepth, duration, searchInfo, finalEval);
    std::string sourceSquare = squareToAlgebraic(selectedMove.source);
    std::string destSquare = squareToAlgebraic(selectedMove.dest);
    std::string strMove = sourceSquare + destSquare + promotionChar(selectedMove);

    std::cout << "info score cp " << finalEval << std::endl;
    std::cout << "bestmove " << strMove << std::endl;

}
