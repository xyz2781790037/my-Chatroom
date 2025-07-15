#include "../netlib/net/TcpServer.h"
#include <nlohmann/json.hpp>
#include "handleData.h"
void onConnection(const mulib::net::TcpConnectionPtr &conn)
{
    if (conn->connected()){
        std::cout << "new conncetion" << std::endl;
    }
}
void onMessage(const TcpConnectionPtr &conn, Buffer *buf, mulib::base::Timestamp recviveTime, std::shared_ptr<handleData> &handledata_)
{
    handledata_->Megcycle(conn, buf);
}
int main(){
    auto handledata_ = std::make_shared<handleData>();
    mulib::net::EventLoop mainLoop;
    mulib::net::InetAddress addr(8080);
    mulib::net::TcpServer server(&mainLoop, "G", addr);
    server.setThreadNum(32);
    server.setConnectionCallback(onConnection);
    server.setMessageCallback([&handledata_](const TcpConnectionPtr &conn, Buffer *buf, mulib::base::Timestamp recviveTime){
        onMessage(conn,buf,recviveTime,handledata_);
    });
    server.start();
    mainLoop.loop(-1);
    return 0;
}