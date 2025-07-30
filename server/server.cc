#include "../netlib/net/TcpServer.h"
#include <nlohmann/json.hpp>
#include "handleData.h"
#include "../base/threadPool.h"
void onConnection(const mulib::net::TcpConnectionPtr &conn)
{
    if (conn->connected()){
        std::cout << "new conncetion" << std::endl;
    }
}
void onMessage(const TcpConnectionPtr &conn, mulib::base::Timestamp recviveTime, std::shared_ptr<handleData> &handledata_, MessageSplitter &megser, redisCmd &redis)
{
    handledata_->Megcycle(conn, megser,redis);
}
int main(){
    ThreadPool pool(128);
    auto handledata_ = std::make_shared<handleData>();
    mulib::net::EventLoop mainLoop;
    mulib::net::InetAddress addr(8080);
    mulib::net::TcpServer server(&mainLoop, "GChat", addr);
    server.setThreadNum(32);
    server.setConnectionCallback(onConnection);
    server.setMessageCallback([&handledata_, &pool](const TcpConnectionPtr &conn, Buffer *buf, mulib::base::Timestamp recviveTime) {
        std::string msg = buf->retrieveAllAsString();

        pool.enqueue([conn, recviveTime, &handledata_, msg]
                     {
            thread_local redisCmd redis;
            MessageSplitter megser;
            megser.append(msg);
            onMessage(conn,recviveTime,handledata_,megser,redis); });
     });
    server.start();
    mainLoop.loop(-1);
    return 0;
}