#include "netClient.hpp"

void serverListener(
    NetConnection clientCon,
    std::atomic<bool>& recvFlag,
    struct Packet& recvPacket,
    std::mutex& m
) {
    try {
        int netStatus;
        std::string response;
        while (true) {
            response = clientCon.netGetLine(0, netStatus); 
            if (netStatus <= 0) {
                //Server disconnected or error occurred
                std::cerr << "serverListener breaking loop, received 0 bytes in recv()\n";
                if (netStatus == -1)
                    std::cerr << "errno=" << errno << std::endl;
                break;
            }
            std::cout << "recv loaded " << response << std::endl;
            std::unique_lock lk(m);
            recvFlag = true;
            std::strncpy(recvPacket.str, response.c_str(), response.length());
            //Could cause a crash if the response is longer than buffer //TODO - remove struct Packet, just use std::string
            recvPacket.str[response.length()] = '\0';

            if (std::string(recvPacket.str) == "bye") {
                lk.unlock();
                break;
            }
            lk.unlock();
        }
    } catch (std::system_error& e) {
        std::cerr << "Blew up in serverListener" << std::endl;
        std::exit(-1);
    }
}

void humanServerListener(
    const int socketFD,
    BoardState& boardState,
    std::atomic<bool>& guiNeedsToMove,
    std::atomic<bool>& gameOver,
    std::mutex& m,
    std::condition_variable& cv 
){
    while(!gameOver){
        char buf[PACKET_STR_SIZE];
        int bytesRead = recv(socketFD, (void *)buf, PACKET_STR_SIZE - 1, 0);
        if (bytesRead <= 0) {
            //Server disconnected or error occurred
            std::cerr << "serverListener breaking loop, received 0 bytes in recv()\n";
            gameOver = true;
            if (bytesRead == -1)
                std::cerr << "errno=" << errno << std::endl;
            break;
        }
        buf[bytesRead] = '\0';
        std::string response = std::string(buf);
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
    const int socketFD,
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
        send(socketFD, UCIMove.c_str(), UCIMove.length(), 0);

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
