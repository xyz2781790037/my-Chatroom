#include <../netlib/net/EventLoop.h>
#include "../netlib/net/TcpConnection.h"
#include "../base/logOn.h"
#include "handleMeg.h"
int main(){
    mulib::base::Logger::setLogLevel(mulib::base::Logger::DEBUG);
    mulib::net::EventLoop mainLoop;
    mulib::net::InetAddress addr("127.0.0.1", 8080); // 10.30.0.127
    auto clientPtr = std::make_shared<mulib::net::TcpClient>(&mainLoop, addr);
    clientPtr->connect();
    logon clientLog(clientPtr);
    clientPtr->setConnectionCallback([&clientLog](const mulib::net::TcpClient::TcpConnectionPtr &conn) {
    if (conn->connected()) {
        std::cout << "连接成功，可以开始交互" << std::endl;
        std::thread uiThread([&clientLog]() mutable
                             { clientLog.ui(); });
        uiThread.detach();
    } else {
        std::cout << "连接断开" << std::endl;
    } });
    handleMeg handlemeg_;
    clientPtr->setMessageCallback([&handlemeg_](const mulib::net::TcpClient::TcpConnectionPtr &conn, Buffer *buf, mulib::base::Timestamp) {  
        LOG_INFO << "收到消息";
        handlemeg_.recviveMeg(conn,buf);
    });
    mainLoop.loop(-1);
    return 0;
}