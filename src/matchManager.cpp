#include "matchManager.hpp"

int main(int argc, char **argv) {
    struct GameState gameState = {
        .gameOver = false,
        .whiteReady = false,
        .blackReady = false,
        .timeUp = false,
        .blackTime = 60*5*1000,
        .whiteTime = 60*5*1000, //default time in ms: 5 minutes
		.blackTotalTime = 60*5*1000,
		.whiteTotalTime = 60*5*1000
    };
    std::string fen = STARTFEN;
	std::string fenDB = "data/fenList.txt";
	unsigned int gamesToPlay = 1;
    for (int i = 0; i < argc; ++i) {
        std::string s = std::string(argv[i]);
        if (s == "-wt") {
    		gameState.whiteTotalTime = std::atoi(argv[i+1]);
            gameState.whiteTime = std::atoi(argv[i+1]);
        } 
        else if (s == "-bt") {
    		gameState.blackTotalTime = std::atoi(argv[i+1]);
            gameState.blackTime = std::atoi(argv[i+1]);
        } 
		else if (s == "--benchmark") {
			gamesToPlay = std::atoi(argv[i+1]);
		}
		else if (s == "-fdb") { // "Fen DataBase"
			fenDB = std::string(argv[i+1]);
		}
        else if (s == "-f") {
            fen = std::string(argv[i+1]);
            int j = i + 2;
            //Finds new arg or reaches the end
            //TODO Fens can have dashes so we need a better way
			//of dealing with this when we add new args
            while (
                std::string(argv[j]).find("-wt") == std::string::npos &&
                std::string(argv[j]).find("-bt") == std::string::npos &&
                std::string(argv[j]).find("-fdb") == std::string::npos &&
                std::string(argv[j]).find("--benchmark") == std::string::npos &&
                j < argc
            ) {
                fen += " ";
                fen += std::string(argv[j]);
                j++;
            }
        } 
    }
    struct GameHistory gameHistory = {
        .moveIndex = 0,
        .startFen = fen,
		.fenDBFilePath = fenDB,
    };

    std::cerr << "fen: " << fen << std::endl;

    //Match Manager needs to be a source of truth for moves
    generateKingAttacks();
    generateKnightAttacks();

    std::thread userMatchManagerThread;
    std::thread userCLIThread;
    std::thread engineOneThread;
    std::thread engineTwoThread;
    std::string UCIResponse; //locked behind the mutex condition
    bool responseReady = false; //locked behind the mutex condition
    
    uint32_t lightColor = 0xffffffff;
    uint32_t darkColor = 0xff4a9627;
    fenToBoardState(fen, std::ref(gameState.state));
    gameState.turnState = gameState.state.sideToMove;


	userCLIThread = std::thread(
		CLIThread,
		std::ref(gameState.gameOver),
		std::ref(gameState.timeUp),
		std::ref(gamesToPlay),
		std::ref(gameState.threadSyncMutex),
		std::ref(gameState.mutexCondition)        
	);


	userMatchManagerThread = std::thread(
		matchManagerThread,
		std::ref(gameState),
		std::ref(gameHistory),
		gamesToPlay,
		std::ref(responseReady),
		std::ref(UCIResponse)
	);
	engineOneThread = std::thread(
		engineThread,
		std::ref(gameState),
		std::ref(gameHistory),
		WHITE,
		std::ref(UCIResponse),
		std::ref(responseReady)
	);
	engineTwoThread = std::thread(
		engineThread,
		std::ref(gameState),
		std::ref(gameHistory),
		BLACK,
		std::ref(UCIResponse),
		std::ref(responseReady)
	);

	//This will be unused for matchManager
	std::string lastMoveMade;
	renderBoard(
		std::ref(gameState.state),
		std::ref(gameState.gameOver),
		darkColor,
		lightColor,
		"img",
		-1,
		std::ref(lastMoveMade),
		std::ref(gameState.threadSyncMutex),
		std::ref(gameState.mutexCondition) //This will be unused for matchManager
	);

	//If game ends because of gui close
	gameState.gameOver = true;
	gameState.mutexCondition.notify_all();

	userMatchManagerThread.join();
	engineOneThread.join();
	engineTwoThread.join();
    userCLIThread.detach();
}

void engineThread(
    struct GameState& gameState,
    struct GameHistory& gameHistory,
    int color,
    std::string& UCIResponse,
    bool& responseReady
) {
    int sockDesc = -1;
    if ((sockDesc = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0)) == -1) {
        std::cerr << "matchManager - Failed to create socket in engineThread: color = " << color << std::endl;
    }
    struct sockaddr_in listenAddressOne = {
        .sin_family = AF_INET,
		//Color *must* be 0 or 1 (white, black) 
        .sin_port = htons(ENGINE_LISTEN_PORT[color]),
        .sin_addr = {.s_addr = INADDR_ANY} //(man 7 ip)
    };
    const int reuse = 1;
    setsockopt(
		sockDesc,
		SOL_SOCKET,
		SO_REUSEADDR,
		(void *) &reuse,
		sizeof(reuse)
	);
    int res = bind(
		sockDesc,
		(const struct sockaddr *)&listenAddressOne,
		sizeof(listenAddressOne)
	);
    if (res == -1) {
        std::cerr << "matchManager - Failed to bind socket. Errno=" << errno << std::endl;
        return;
    }

    const int connectionBacklogLimit = 1;
    listen(sockDesc, connectionBacklogLimit);
    struct sockaddr_in clientConnInfo; //filled in with call to accept()
	// \/ will be overwritten by accept()
    socklen_t connSizeInfo = sizeof(clientConnInfo); 
	int clientDesc = -1;
	do {
        if (gameState.gameOver) { return; }
        clientDesc = accept(
			sockDesc,
			(struct sockaddr *) &clientConnInfo,
			&connSizeInfo
		);
        if (clientDesc == -1) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                continue;
            } else {
                std::cerr << "matchManager - Engine Thread (color=" << color << ") failed accept(), errno=" << errno << std::endl;
                return; //Probably should gracefully close down the threads and such at some point
                //For now, just returning early. TODO
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100)); //TODO - magic number
    } while (clientDesc == -1);
    NetConnection clientConnection(clientDesc);
    clientConnection.setBlocking();
    if (color == WHITE) {
        gameState.whiteReady = true;
    }
    else {
        gameState.blackReady = true;
    }
    gameState.mutexCondition.notify_all();
    std::cerr << "matchManager - Client successfully connected: "
        << ntohs(clientConnInfo.sin_addr.s_addr)
        << ":" << ntohs(clientConnInfo.sin_port) << std::endl;
    
    clientConnection.netSend("uci\n");
    int netStatus;
    std::string cppBuf;
    do {
        cppBuf = clientConnection.netGetLine(0, std::ref(netStatus));
        if(netStatus == 0){
            std::cerr << "Engine disconnected during UCI handshake" << std::endl;
            clientConnection.netClose();
            close(sockDesc);
            return;
        }
		else if(netStatus == -1) {
			std::cerr << "Engine error during UCI handshake, errno=" << errno
				<< std::endl;
			clientConnection.netClose();
			close(sockDesc);
			return;
		}
    } while (cppBuf.find("id name") == std::string::npos);
    std::cerr << "{{{ " << cppBuf << " }}}" << std::endl << std::endl;
    if (netStatus > 0) {
        std::istringstream ss(cppBuf);
        std::string token;
        while (ss >> token) {
            if (token == "name") {
                std::string engineName;
                std::getline(ss, engineName);

                if (color == WHITE) {
                    gameHistory.whiteName = engineName;
                }
                else {
                    gameHistory.blackName = engineName;
                }
            }
        }
    } else {
        std::cerr << "Failed to get info after uci command sent" << std::endl;
    }

    if (color == WHITE) {
        std::cerr << "white's name = " << gameHistory.whiteName << std::endl;
    }
    else {
        std::cerr << "black's name = " << gameHistory.blackName << std::endl;
    }

    clientConnection.netSend("isready\n");

    //draining all data from pending recv queue
    //Checking for readyok
    do { 
        cppBuf = clientConnection.netGetLine(0, netStatus);
		if (netStatus == 0 || netStatus == -1) {
			std::cerr << "Failed while waiting for readyok" << std::endl;
			clientConnection.netClose();
			close(sockDesc);
			return;
		}
    } while(cppBuf.find("readyok") == std::string::npos);

    while (true) {
        std::unique_lock lk(gameState.threadSyncMutex);
        gameState.mutexCondition.wait(lk, [color, &gameState, &responseReady]{ 
                return (!responseReady && (color == gameState.turnState) && gameState.whiteReady && gameState.blackReady) || gameState.gameOver; 
        }); //my turn
        std::cerr << "matchManager engine thread woken up, color=" << color << std::endl;
        lk.unlock(); //This allows main thread to continue to act
        if(gameState.gameOver){
            clientConnection.netSend("bye");
            clientConnection.netClose();
            close(sockDesc);
			return;
        }

        std::string cmd;
        lk.lock();
        cmd = createPositionCmd(gameState.state) + "\n";
        lk.unlock();
        assert(cmd.length() <= PACKET_STR_SIZE); //Need some form of bounds checking
            //better to crash than error silently

        //send Position command
        std::cerr << "matchManager DEBUG: transmitting {" << cmd << "}" << std::endl;
        clientConnection.netSend(cmd);
        
        cmd = "go wtime " + std::to_string(gameState.whiteTime) + " btime " + std::to_string(gameState.blackTime) + "\n";
        //send Go command
        std::cerr << "matchManager DEBUG2: transmitting {" << cmd << "}" << std::endl;
        clientConnection.netSend(cmd);
        //await engine response
        //int bytesRead;
        std::string response = "";
        //char buf[PACKET_STR_SIZE] = {0};
        while(1) {
            //std::memset((void *) buf, 0, PACKET_STR_SIZE); //clearing buffer for sanity's sake
            //bytesRead = recv(clientDesc, buf, PACKET_STR_SIZE - 1, MSG_DONTWAIT);
            //clientConnection.netRecv(MSG_DONTWAIT);
            //empty the queue until we find bestmove
            response = clientConnection.netGetLine(MSG_DONTWAIT, netStatus);
            if (netStatus == -1 && (errno != EWOULDBLOCK && errno != EAGAIN)) {
				std::cerr << "Error in mid-game, errno=" << errno << std::endl;
				clientConnection.netClose();
				close(sockDesc);
				return;
			}
            else if(netStatus == 0){
                std::cerr << "Engine disconnected mid-game" << std::endl;
                clientConnection.netClose();
				close(sockDesc);
                return;
            }

            if(response.find("bestmove") != std::string::npos){
                break;
            }
            if (gameState.gameOver) {
                const int res = clientConnection.netSend("bye\n");
                std::cerr << "res = " << res << std::endl;
                clientConnection.netClose();
                close(sockDesc);
                return;
            }
            else if (gameState.timeUp) {
                const int res = clientConnection.netSend("stop\n");
                if (res == -1) {
                    std::cerr << "Failed to send time up signal" << std::endl;
					//TODO - diagnose reason for failed send, handle.
                }
                gameState.timeUp = false;
                continue;
            }
            if(response.length() == 0){
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
            //TODO -- can later parse the other info the engines send in here
            //such as what the engine thought of a certain position, etc
        }

        std::cerr << response << std::endl;
        lk.lock(); //This ensure that it is safe to write to the global UCIResponse
                   //Would it make sense to just make UCIResponse non-global, and passed
                   //as an atomic value to the threads that need to access it?
        //send received move to main thread
        UCIResponse = response;
        std::cerr << "UCIResponse from thread (color=" << color << "):" << UCIResponse << std::endl;
        responseReady = true;
        lk.unlock();
        gameState.mutexCondition.notify_all();
    }
    clientConnection.netClose();
	close(sockDesc);
    return;
}

void CLIThread(std::atomic<bool>& gameOver, std::atomic<bool>& timeUp, unsigned int& gamesToPlay, std::mutex& m, std::condition_variable& cv){
    while(true){
        std::string userInput;
        std::cin >> userInput;
        std::cout << userInput;
        if(userInput == "abort" || std::cin.eof() /*|| gameOver == true*/){
            std::unique_lock lk(m);
			gamesToPlay = 0;
            gameOver = true;
            lk.unlock();
            cv.notify_all();
            break;
        }
        else if (userInput == "stop") { //tells the current engine to immediately stop thinking and send its current best move
            timeUp = true;
        }
        else{
            std::cout << "Unknown Command: " << userInput << std::endl;
        }
    }
}

void matchManagerThread(
    struct GameState& gameState,
    struct GameHistory& gameHistory,
	int gamesToPlay,
    bool& responseReady,
    std::string& UCIResponse
){
	auto timeBeforeMove = std::chrono::steady_clock::now();
	auto timeStart = timeBeforeMove;
    while(true){
        std::unique_lock lk(gameState.threadSyncMutex);
        gameState.mutexCondition.wait(lk, [&gameState, &responseReady]{ return gameState.gameOver || responseReady;});
        if(gameState.gameOver){
            lk.unlock();
            //Dont need to notify because CLIThread already woke up other threads
            break;
        }
		auto timeAfterMove = std::chrono::steady_clock::now();

		auto timeDiff = std::chrono::duration_cast<std::chrono::milliseconds>(timeAfterMove - timeBeforeMove);
        if (timeBeforeMove != timeStart) {
            if (gameState.turnState == WHITE) {
                gameState.whiteTime -= timeDiff.count();
            } else {
                gameState.blackTime -= timeDiff.count();
            }
        }

        if(UCIResponse.find("bestmove") != std::string::npos){
            std::istringstream ss(UCIResponse);
            std::string token;
            //Skip unti we reach "bestmove"
            //Not the cleanest fix had to add this to deal with the engine thread sending: 
            //"info score cp 0bestmove b1c3"
            //Should probably figure out why the engine is sending info and bestmove together
            do{
                ss >> token;
            } while(token.find("bestmove") == std::string::npos);
            //token equals move ie "e2e4"
            ss >> token;

            std::cerr << "move token: " << token << std::endl;

            Move engineMove = strMoveToMove(token, gameState.state);

            std::cerr << "engineMove.src: " << engineMove.source << std::endl << "engineMove.dest: " << engineMove.dest << std::endl;

            UndoState undo;
            makeMove(gameState.state, engineMove, undo);
			gameHistory.undoStates[gameHistory.moveIndex] = undo;
			gameHistory.moves[gameHistory.moveIndex] = engineMove;
			gameHistory.moveFens[gameHistory.moveIndex++] = boardStateToFen(gameState.state);
			
            std::cerr << "After makeMove call: " << boardStateToFen(gameState.state) << std::endl;
            UCIResponse = "";
            responseReady = false;
            gameState.turnState = gameState.state.sideToMove;
            MoveList legalMoves = generateLegalMoves(gameState.state);
            //Checks if game is over
            if(legalMoves.count == 0){
                int defenderSide = gameState.state.sideToMove;
                int attackerSide = (defenderSide == WHITE) ? BLACK : WHITE;
                int kingIndex = __builtin_ctzll(gameState.state.pieces[defenderSide][KING]);
                if (isSquareAttacked(gameState.state, kingIndex, attackerSide)) {
                    gameHistory.winner = attackerSide;
                }
                else {
                    gameHistory.winner = DRAW;
                }

                gameHistoryToFile(gameHistory, "data/game_history.txt");

                if (--gamesToPlay == 0) {
                    gameState.gameOver = true;
                    lk.unlock();
                    gameState.mutexCondition.notify_all();
                    break;
                }
                else {
                    //Reset gameState and gameHistory
                    std::string fen = getRandomFen(gameHistory.fenDBFilePath);
                    std::cerr << "setting fen to: "<<fen<<std::endl;
                    gameState.timeUp = false;
                    long int whiteTotalTime = gameState.whiteTime;
                    gameState.whiteTime = whiteTotalTime; 
                    long int blackTotalTime = gameState.blackTotalTime;
                    gameState.blackTime = blackTotalTime;
                    fenToBoardState(fen, gameState.state);
                    gameState.turnState = gameState.state.sideToMove;
                    gameHistory.moveIndex = 0;
                    gameHistory.startFen = fen;
				}
            }
            timeBeforeMove = std::chrono::steady_clock::now();
            lk.unlock();
            gameState.mutexCondition.notify_all();
        }
        else{
            lk.unlock();
        }
    }
}

void gameHistoryToFile(struct GameHistory& history, const std::string filename) {
	std::ofstream file(filename, std::ios::app);
	file << "White Player name: " << history.whiteName << std::endl;
	file << "Black Player name: " << history.blackName << std::endl;
	file << "Starting fen: " << history.startFen << std::endl;
	if (history.winner == DRAW) {
		file << "Game Result: Draw" << std::endl;
	}
	else {
		file << "Game Result: " << ((history.winner == WHITE) ? "White" : "Black") << " wins!" << std::endl;
	}

	for (unsigned int i = 0; i < history.moveIndex; ++i) {
		file << "fen of position #" << i << " " << history.moveFens[i] << std::endl;
		file << "move of position #" << i << " " << moveToStrMove(history.moves[i]) << std::endl;
	}
	file << std::endl << std::endl << "=================================================================================================" << std::endl << std::endl;
	file.close();
}
