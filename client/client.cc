#include <../netlib/net/EventLoop.h>
#include "../netlib/net/TcpConnection.h"
#include "handleMeg.h"
int main(){
    mulib::base::Logger::setLogLevel(mulib::base::Logger::DEBUG);
    mulib::net::EventLoop mainLoop;
    mulib::net::InetAddress addr("10.30.0.127", 8080);
    auto clientPtr = std::make_shared<mulib::net::TcpClient>(&mainLoop, addr);
    clientPtr->connect();
    logon clientLog(clientPtr);
    clientPtr->setConnectionCallback([&clientLog](const mulib::net::TcpClient::TcpConnectionPtr &conn)
                                 {
    if (conn->connected()) {
        std::cout << "连接成功，可以开始交互" << std::endl;
        std::thread uiThread([&clientLog]() mutable
                             { clientLog.ui(); });
        uiThread.detach();
    } else {
        std::cout << "连接断开" << std::endl;
    } });
    handleMeg handlemeg_;
    clientPtr->setMessageCallback([&clientLog,&handlemeg_](const mulib::net::TcpClient::TcpConnectionPtr &conn, Buffer *buf, mulib::base::Timestamp) {  
        handlemeg_.recviveMeg(conn,buf,clientLog);
    });
    mainLoop.loop(-1);
    return 0;
}