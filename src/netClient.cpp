#include "netClient.hpp"

void serverListener(
    NetConnection clientCon,
    std::atomic<bool>& recvFlag,
    std::string& recvStr,
    std::mutex& m
) {
    int netStatus;
    std::string response;
    while (true) {
        response = clientCon.netGetLine(0, netStatus); 
        if (netStatus <= 0) {
            //Server disconnected or error occurred
            std::cerr << "serverListener breaking loop, received 0 bytes in recv()\n";
            if (netStatus == -1)
                std::cerr << "errno=" << errno << std::endl;
            clientCon.netClose();
            std::unique_lock lk(m);
            recvStr = "bye";
            lk.unlock();
            return;
        }
        std::cout << "recv loaded " << response << std::endl;
        std::unique_lock lk(m);
        recvFlag = true;
        recvStr = response;

        if (recvStr.find("bye") != std::string::npos) {
            lk.unlock();
            break;
        }
        lk.unlock();
    }
}

void humanServerListener(
    NetConnection clientCon,
    BoardState& boardState,
    std::atomic<bool>& guiNeedsToMove,
    std::atomic<bool>& gameOver,
    std::mutex& m,
    std::condition_variable& cv 
){
    while(!gameOver){
        int netStatus;
        std::string response = clientCon.netGetLine(0, netStatus);
        if (netStatus <= 0) {
            //Server disconnected or error occurred
            std::cerr << "serverListener breaking loop, received 0 bytes in recv()\n";
            gameOver = true;
            if (netStatus == -1)
                std::cerr << "errno=" << errno << std::endl;
            break;
        }
        if(response.find("position") != std::string::npos){
            std::unique_lock lk(m);
            applyPositionCommand(response, boardState);
            lk.unlock();
        }
        if(response.find("go") != std::string::npos){
            std::cerr << "guiNeedsToMove" << std::endl;
            guiNeedsToMove = true;
            cv.notify_all();
        }
        if(response.find("bye") != std::string::npos){
            gameOver = true;
            return;
        }
    }
}

void humanSender(
    NetConnection clientCon,
    std::string& moveMadeStr,
    std::atomic<bool>& guiNeedsToMove,
    std::atomic<bool>& gameOver,
    std::mutex& m,
    std::condition_variable& cv
){
    while(!gameOver){
        std::unique_lock lk(m);
        cv.wait(lk, [&moveMadeStr, &guiNeedsToMove, &gameOver]{
            return gameOver || (!moveMadeStr.empty() && guiNeedsToMove);       
        });
        std::cerr << "humanSender woken up" << std::endl;
        if(gameOver){
            cv.notify_all();
            return;
        }
        std::string UCIMove = "bestmove " + moveMadeStr + "\n";
        std::cerr << "Human played: " + UCIMove;
		clientCon.netSend(UCIMove);

        moveMadeStr = "";
        guiNeedsToMove = false;
        lk.unlock();
    }
}

void applyPositionCommand(const std::string& cmd, BoardState& boardState) {
    std::istringstream ss(cmd);
    std::string token;
    
    //Skip "position"
    ss >> token;

    ss >> token;

    if (token == "startpos") {
        fenToBoardState(STARTFEN, boardState);
        //Read the next token, which might be "moves"
        ss >> token;
    }
    else if (token == "fen") {
        std::string fen = "";
        int fenPartsCount = 0;

        // A FEN string is exactly 6 parts separated by spaces.
        // We pull those 6 parts, and stop if we hit "moves"
        while (fenPartsCount < 6 && ss >> token && token != "moves") {
            fen += token + " ";
            fenPartsCount++;
        }

        // Remove the trailing space
        if (!fen.empty()) {
            fen.pop_back();
        }
        fenToBoardState(fen, boardState);
    }

    // Now, if the last token we pulled was "moves", we apply them sequentially
    if (token == "moves") {
        while (ss >> token) {
            Move move = strMoveToMove(token, boardState);
            UndoState undo;
            if (move.raw != 0) {
                makeMove(boardState, move, undo);
            } else {
                std::cerr << "HumanClient - Failed to parse move from MatchManager: " << token << std::endl;
            }
        }
    }
}
