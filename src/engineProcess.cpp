#include "engineProcess.hpp"

EngineProcess::EngineProcess(const std::string& enginePath){
    if(pipe(read_pipe) == -1 || pipe(write_pipe) == -1){
        throw std::runtime_error("Failed to create pipes");
    }
    
    child_pid = fork();

    if(child_pid == -1){
        throw std::runtime_error("Failed to fork");
    }

    if(child_pid == 0){
        //Child Process (Engine)
        
        //Redirect stdin to write_pipe[0] (aka the read end of the write pipe)
        dup2(write_pipe[0], STDIN_FILENO);
        //Redirect stdout to read_pipe[1] (aka the write end of the read pipe)
        dup2(read_pipe[1], STDOUT_FILENO);

        close(read_pipe[0]);
        close(read_pipe[1]);
        close(write_pipe[0]);
        close(write_pipe[1]);

        execl(enginePath.c_str(), enginePath.c_str(), nullptr);

        std::cerr << "Fatal Error: Failed to launch engine." << std::endl;
        exit(1);
    }
    else{
        //Parent Process gui

        //Parent does not use the write end of read_pipe
        //or read end of write_pipe
        close(read_pipe[1]);
        close(write_pipe[0]);

        // Make the read pipe NON-BLOCKING so SDL doesn't freeze
        int flags = fcntl(read_pipe[0], F_GETFL, 0);
        fcntl(read_pipe[0], F_SETFL, flags | O_NONBLOCK);
    }
}

EngineProcess::~EngineProcess(){
    sendCommand("quit");
    close(read_pipe[0]);
    close(write_pipe[1]);
    waitpid(child_pid, nullptr, 0); // Wait for engine to cleanly exit
}

void EngineProcess::sendCommand(const std::string& cmd){
    std::string fullCmd = cmd + "\n";
    if(write(write_pipe[1], fullCmd.c_str(), fullCmd.length()) == -1){
        std::cerr << "Falied to write command: " << fullCmd;
    }

}

std::string EngineProcess::receiveCommand(){
    if(!hasData()){
        return "";
    }
    size_t pos = buffer.find('\n');
    std::string line = buffer.substr(0, pos);
    buffer.erase(0, pos + 1);

    return line;
}

bool EngineProcess::hasData(){
    char tmp_buf[256];

    ssize_t bytesRead = read(read_pipe[0], tmp_buf, sizeof(tmp_buf) - 1);
    if(bytesRead > 0){
        tmp_buf[bytesRead] = '\0';
        buffer.append(tmp_buf, bytesRead);
    }

    return buffer.find('\n') != std::string::npos;
}

bool EngineProcess::initCommunication() {
    sendCommand("uci");
    std::string engineResponse = receiveCommand();

    do {
        if (engineResponse != "") {
            std::cout << "Engine says: " << engineResponse << std::endl;
        }
    } while ((engineResponse = receiveCommand())  != "uciok");

    sendCommand("isready");

    do {
        if (engineResponse != "") {
            std::cout << "Engine says: " << engineResponse << std::endl;
        }
    } while ((engineResponse = receiveCommand())  != "readyok");

    sendCommand("ucinewgame");
    return true;
    //TODO - actually handle not establishing communication / avoid infinite loop on failure
}
