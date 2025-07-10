#include "../netlib/net/TcpServer.h"
#include <nlohmann/json.hpp>
#include "../base/handleData.h"
#include "../base/MessageSplitter.h"
void onConnection(const mulib::net::TcpConnectionPtr &conn)
{
    if (conn->connected())
    {
        std::cout << "new conncetion" << std::endl;
    }
}
void onMessage(const TcpConnectionPtr&conn ,Buffer *buf,mulib::base::Timestamp recviveTime){
    handleData handledata_;
    redisCmd redis;
    MessageSplitter megser;
    std::string json_str = buf->retrieveAllAsString();
    LOG_INFO << "收到数据 :" << json_str;
    megser.append(buf);
    std::string jsondata;
    while(megser.nextMessage(jsondata)){
        auto jsonData = nlohmann::json::parse(jsondata);
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