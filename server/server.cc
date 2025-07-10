#include "../netlib/net/TcpServer.h"
#include <nlohmann/json.hpp>
#include "../base/handleData.h"
void onConnection(const mulib::net::TcpConnectionPtr &conn)
{
    if (conn->connected())
    {
        std::cout << "new conncetion" << std::endl;
    }
}
nlohmann::json onMessage(const TcpConnectionPtr&conn ,Buffer *buf,mulib::base::Timestamp recviveTime){
    std::string json_str = buf->retrieveAllAsString();
    LOG_INFO << "收到数据 :" << json_str;
    return nlohmann::json::parse(json_str);
}
int main(){
    redisCmd redis;
    handleData handledata_;
    mulib::net::EventLoop mainLoop;
    mulib::net::InetAddress addr(8080);
    mulib::net::TcpServer server(&mainLoop, "G", addr);
    server.setThreadNum(32);
    server.start();
    server.setConnectionCallback(onConnection);
    server.setMessageCallback([&redis,&handledata_](const TcpConnectionPtr &conn, Buffer *buf, mulib::base::Timestamp recviveTime){
        nlohmann::json jsonData = onMessage(conn,buf,recviveTime);
        if (jsonData.contains("type")){
            LOG_DEBUG << "JSON 正常";
            int type = handledata_.getDataType(jsonData["type"]);
            if(type == handleData::REGISTER){
                handledata_.handleRegister(conn, jsonData, redis);
            }
            else if (type == handleData::LOGIN)
            {
                handledata_.handleLogin(conn, jsonData, redis);
            }
        }
        else
        {
            LOG_ERROR << "JSON 数据缺少必要字段，忽略";
        }
    });
    mainLoop.loop(-1);
    return 0;
}