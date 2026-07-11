#include "netUtils.hpp"

int NetConnection::netRecv(){
    return 0;

}

int NetConnection::netSend(std::string msg){
    int res = send(sockfd, msg.c_str(), msg.length(), 0);
    validateSend(res, msg.length());
    return res;
}

int NetConnection::netClose(){
    return close(sockfd);

}

std::string NetConnection::dequeue(){
    if(recvQueue.empty()){
        return "";
    }
    std::string response = recvQueue.front();
    recvQueue.pop();
    return response;

}



void validateSend(int sendRetVal, int expectedByteCount) {
    if (sendRetVal == -1) {
        std::cerr << "DEBUG ERROR: matchManager - failed to send cmd string. errno="
            << errno << std::endl;
        return;
        //TODO - probably should have some sort of error function that shuts down
        //all the components of the program cleanly, and can be called from anywhere.
    } else if (sendRetVal != expectedByteCount) {
        std::cerr << "DEBUG INFO: matchManager - socket bytes sent isn't expected value"
            << "continuing execution..." << std::endl;
    } else { }
}

