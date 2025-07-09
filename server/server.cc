#include "../netlib/net/TcpServer.h"
#include <../netlib/base/logger.h>
#include <redisCmd.h>
void onConnection(const mulib::net::TcpConnectionPtr &conn)
{
    if (conn->connected())
    {
        std::cout << "new conncetion" << std::endl;
        // conn->send("hello!");
    }
}
void onMessage(const TcpConnectionPtr&conn ,Buffer *buf,mulib::base::Timestamp recviveTime){
    std::string json_str = buf->retrieveAllAsString();
    LOG_INFO << "收到数据 :" << json_str;
}
int getDataType(){

}
int main(){
    redisCmd redis;
    mulib::net::EventLoop mainLoop;
    mulib::net::InetAddress addr(8080);
    mulib::net::TcpServer server(&mainLoop, "G", addr);
    server.setThreadNum(32);
    server.start();
    server.setConnectionCallback(onConnection);
    server.setMessageCallback(onMessage);
    mainLoop.loop(-1);
    return 0;
}