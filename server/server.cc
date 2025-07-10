#include "../netlib/net/TcpServer.h"
#include <../netlib/base/logger.h>
#include <redisCmd.h>
#include <nlohmann/json.hpp>
#include "handleData.h"
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
        if (jsonData.contains("account") && jsonData.contains("password")){
            LOG_DEBUG << "JSON 正常";
            int type = handledata_.getDataType(jsonData["type"]);
            if(type == 0){
                redis.setNewUser(jsonData);
                LOG_INFO << "准备发送注册成功消息给客户端";
                if (conn && conn->connected())
                {
                    conn->send("注册成功！");
                    LOG_INFO << "end";
                }
                else
                {
                    std::cout << "连接尚未建立，无法发送注册信息。\n";
                }
            }
            else if(type == 1){
                int result = redis.handleLogin(jsonData);
                if(result == 1){
                    redis.returnUser(jsonData);
                    conn->send(jsonData.dump());
                    conn->send("成功登陆");
                }
                else if(result == 0){
                    conn->send("密码错误");
                }
                else{
                    conn->send("账号不存在");
                }
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