#include "engine.hpp"

int main(){
    BoardState boardState;
    fenToBoardState("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", boardState);
    std::string line;
    srand(time(NULL));
    generateKingAttacks();
    generateKnightAttacks();
    initZobristTable("zobristKeys.json");

    while(std::getline(std::cin, line)){
        if(line == "quit"){
            break;
        }
        else if(line == "uci"){
            std::cout << "id name Graggle Chess" << std::endl;
            std::cout << "id author Jacob Todd" << std::endl;
            std::cout << "uciok" << std::endl;
        }
        else if(line == "go"){
            /*MoveList legalMoves = generateLegalMoves(boardState);
            int randMove = rand() % legalMoves.count;
            Move selectedMove = legalMoves.moves[randMove];*/
            int finalEval;

            Move selectedMove = searchBestMove(boardState, 6, finalEval);
            std::string sourceSquare = squareToAlgebraic(selectedMove.source);
            std::string destSquare = squareToAlgebraic(selectedMove.dest);
            std::string strMove= sourceSquare + destSquare;

            std::cout << "bestmove " << strMove << " Score: " << finalEval << std::endl;
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
        
    }

    return 0;
}
