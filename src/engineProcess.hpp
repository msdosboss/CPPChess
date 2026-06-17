#ifndef ENGINEPROCESS_H
#define ENGINEPROCESS_H

#include "UCIClient.hpp"
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <iostream>
#include <vector>
#include <stdexcept>

class EngineProcess : public IChessPlayer {
private:
    pid_t child_pid;
    int write_pipe[2];
    int read_pipe[2];
    std::string buffer;

public:
    EngineProcess(const std::string& enginePath);
    ~EngineProcess();
    void sendCommand(const std::string& cmd) override;
    std::string receiveCommand() override;
    bool hasData() override;

};

#endif
