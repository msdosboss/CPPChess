#include "netUtils.hpp"

NetConnection::NetConnection(int sock){
    sockfd = sock;
    recvQueue = std::queue<std::string>();
    accumulatedResponse = std::string();
}

int NetConnection::netRecv(int flags){
    char buf[PACKET_STR_SIZE];
    int bytesRecv = recv(sockfd, buf, PACKET_STR_SIZE - 1, flags);
    if(bytesRecv > 0){
        buf[bytesRecv] = '\0';
        accumulatedResponse.append(buf, bytesRecv);
        size_t newlinePos;
        while((newlinePos = accumulatedResponse.find('\n')) != std::string::npos){
            //This will include the newline
            std::string token = accumulatedResponse.substr(0, newlinePos + 1);
            recvQueue.push(token);
            accumulatedResponse.erase(0, newlinePos + 1);
        }
    }
    return bytesRecv;
}

int NetConnection::netSend(std::string msg){
    int res = send(sockfd, msg.c_str(), msg.length(), 0);
    validateSend(res, msg.length());
    return res;
}

int NetConnection::netClose(){
    int res = close(sockfd);

    if(res != 0){
        std::cerr << "socket failed to close. errno=" << errno << std::endl;
    }

    return res;

}

std::string NetConnection::netDequeue(){
    if(recvQueue.empty()){
        return "";
    }
    std::string response = recvQueue.front();
    recvQueue.pop();
    return response;
}

std::string NetConnection::netGetLine(int flags, int& status){
    //Default to ok
    status = 1;

    if(recvQueue.empty()){
        status = netRecv(flags);
        if(status <= 0){
            return "";
        }
    }

    return netDequeue();
}

void NetConnection::unsetBlocking(){
    int flags = fcntl(sockfd, F_GETFL);
    flags |= O_NONBLOCK;
    fcntl(sockfd, F_SETFL, flags);
}

void NetConnection::setBlocking(){
    int flags = fcntl(sockfd, F_GETFL);
    flags &= ~(O_NONBLOCK);
    fcntl(sockfd, F_SETFL, flags);
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

