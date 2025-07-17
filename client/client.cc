#include <../netlib/net/EventLoop.h>
#include "../netlib/net/TcpConnection.h"
#include "../base/logOn.h"
#include "handleMeg.h"
#include "1.h"
int main()
{

    // mulib::base::Logger::setLogLevel(mulib::base::Logger::DEBUG);
    TermiosGuard guard;
    mulib::net::EventLoop mainLoop;
    mulib::net::InetAddress addr("172.17.0.1", 8080);
    auto clientPtr = std::make_shared<mulib::net::TcpClient>(&mainLoop, addr);

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
        LOG_DEBUG << "收到消息";
        handlemeg_.recviveMeg(conn,buf);
        
    });
    clientPtr->connect();
    mainLoop.loop(-1);
    return 0;
}