#include "../netlib/net/TcpServer.h"
#include <nlohmann/json.hpp>
#include "../base/threadPool.h"
#include "handleFile.h"
void onConnection(const mulib::net::TcpConnectionPtr &conn)
{
    if (conn->connected())
    {
        std::cout << "new conncetion" << std::endl;
    }
}
int main(){
    ThreadPool pool(128);
    auto handleFile_ = std::make_shared<handleFile>();
    auto mainLoop = std::make_shared<mulib::net::EventLoop>();
    mulib::net::InetAddress addr(8088);
    mulib::net::TcpServer server(mainLoop.get(), "FTP", addr);
    server.setThreadNum(32);
    server.setConnectionCallback(onConnection);
    server.setMessageCallback([&pool,&handleFile_,&mainLoop](const TcpConnectionPtr &conn, Buffer *buf, mulib::base::Timestamp recviveTime) {
            std::string msg = buf->retrieveAllAsString();
            LOG_INFO << msg;
            MessageSplitter megser;
            megser.append(msg);
            std::string a;
            handleFile_->getConn(conn, recviveTime);
            while(megser.nextMessage(a))
            handleFile_->handleInput(a);
        });
    server.start();
    mainLoop->loop(-1);
    return 0;
}