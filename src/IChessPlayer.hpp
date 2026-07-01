#ifndef ICHESSPLAYER
#define ICHESSPLAYER

#include <string>

class IChessPlayer {
public:
    virtual ~IChessPlayer() = default;
    virtual void sendCommand(const std::string& cmd) = 0;
    virtual std::string receiveCommand() = 0;
    virtual bool hasData() = 0; 
};


#endif 
