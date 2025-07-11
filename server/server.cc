#include "../netlib/net/TcpServer.h"
#include <nlohmann/json.hpp>
#include "handleData.h"
#include "../base/MegType.h"
#include "../base/MessageSplitter.h"
void onConnection(const mulib::net::TcpConnectionPtr &conn)
{
    if (conn->connected())
    {
        std::cout << "new conncetion" << std::endl;
    }
}
void onMessage(const TcpConnectionPtr &conn, Buffer *buf, mulib::base::Timestamp recviveTime)
{
    redisCmd redis;
    handleData handledata_;
    MessageSplitter megser;
    megser.append(buf);
    std::string jsondata;
    while (megser.nextMessage(jsondata))
    {
        LOG_INFO << jsondata;
        auto jsonData = nlohmann::json::parse(jsondata);
        if (jsonData.contains("type")){
            LOG_DEBUG << "JSON 正常";
            Type::types type = Type::getDataType(jsonData["type"]);
            LOG_INFO << "type is :" << type;
            if (type == Type::REGISTER)
            {
                handledata_.handleRegister(conn, jsonData, redis);
            }
            else if (type == Type::LOGIN)
            {
                handledata_.handleLogin(conn, jsonData, redis);
            }
            else if(type == Type::GETPWD){
                handledata_.returnPwd(conn,jsonData,redis);
            }
        }
        else
        {
            LOG_ERROR << "JSON 数据缺少必要字段，忽略";
        }
    }
}
int main(){
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