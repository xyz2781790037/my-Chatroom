#ifndef HANDERMESSAGE_H
#define HANDERMESSAGE_H
#include "../base/MessageSplitter.h"
#include <../netlib/net/TcpClient.h>
#include "../netlib/base/logger.h"
#include <nlohmann/json.hpp>
#include "../base/MegType.h"
#include "../base/logOn.h"
class handleMeg{
public:
    void recviveMeg(const mulib::net::TcpClient::TcpConnectionPtr &conn, Buffer *buf, logon &clientLog);
private:
    void print(nlohmann::json j);
};
void handleMeg::recviveMeg(const mulib::net::TcpClient::TcpConnectionPtr &conn, Buffer *buf,logon &clientLog)
{
    MessageSplitter megSpl;
    megSpl.append(buf);
    std::string msg;
    while (megSpl.nextMessage(msg))
    {
        LOG_INFO << msg;
        auto jsonData = nlohmann::json::parse(msg);
        Type::types type = Type::getDataType(jsonData["type"]);
        std::cout << "type:" << type << std::endl;
        if (type == Type::PRINT)
        {
            print(jsonData);
        }
        else if (type == Type::GETPWD)
        {
            if (jsonData["return"] == "email")
            {
                std::cout << "你的账号邮箱为：" << jsonData["email"] << std::endl;
                std::cout << "发送验证码到你的邮箱：[Y/n]";
                char ack;
                std::cin >> ack;
                if(ack == 'Y' || ack == 'y'){
                    jsonData["return"] == "verify";
                    conn->send(jsonData.dump() + "\n");
                }
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            }
            else{
                
            }
        }
        clientLog.updataState(jsonData["state"]);
    }
}
void handleMeg::print(nlohmann::json j)
{
    std::cout << "收到服务器消息：" << j["meg"] << std::endl;
    LOG_INFO << "state: " << j["state"];
}
#endif