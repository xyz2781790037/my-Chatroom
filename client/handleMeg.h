#ifndef HANDERMESSAGE_H
#define HANDERMESSAGE_H
#include "../base/MessageSplitter.h"
#include <../netlib/net/TcpClient.h>
#include "../netlib/base/logger.h"
#include <nlohmann/json.hpp>
#include "../base/MegType.h"
#include "../base/user.h"
#include "UserUi.h"
class handleMeg{
public:
    void recviveMeg(const mulib::net::TcpClient::TcpConnectionPtr &conn, Buffer *buf);
private:
    void print(nlohmann::json j);
};
void handleMeg::recviveMeg(const mulib::net::TcpClient::TcpConnectionPtr &conn, Buffer *buf)
{
    MessageSplitter megSpl;
    megSpl.append(buf);
    std::string msg;
    while (megSpl.nextMessage(msg)){
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
            LOG_INFO << "进入getpwd";
            if (jsonData["return"] == "email")
            {
                std::cout << "你的账号邮箱为：" << jsonData["email"] << std::endl;
                std::cout << "已发送验证码到你的邮箱" << std::endl;
                jsonData["return"] = "verify";
                conn->send(jsonData.dump() + "\n");
            }
            else if(jsonData["return"] == "true"){
                std::cout << "你的密码为：" << jsonData["password"] << std::endl;
            }
            Type::updataState(jsonData["state"]);
        }
        else if(type == Type::INFOEMATION){
            auto userPtr = std::make_shared<User>(
                jsonData["account"], jsonData["password"], jsonData["email"]);
            userPtr->updataUserInformation(jsonData["myname"],jsonData["ID"]);
            auto ownPtr = std::make_shared<Userui>(userPtr, conn);
            std::thread ownuiThread([ownPtr]() {
                ownPtr->ui();
            });
            
            ownuiThread.join();
        }
    }
}
void handleMeg::print(nlohmann::json j)
{
    std::cout << "收到服务器消息：" << j["meg"] << std::endl;
    if(j.contains("state")){
        LOG_INFO << "state: " << j["state"];
        Type::updataState(j["state"]);
    }
    else if(j.contains("userstate")){
        LOG_INFO << "Ustate: " << j["userstate"];
        Type::updataUserState(j["userstate"]);
    }
}
#endif