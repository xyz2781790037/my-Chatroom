#ifndef HANDERMESSAGE_H
#define HANDERMESSAGE_H
#include "../base/MessageSplitter.h"
#include <../netlib/net/TcpClient.h>
#include "../netlib/base/logger.h"
#include <nlohmann/json.hpp>
#include "../base/MegType.h"
#include "../base/user.h"
#include "UserUi.h"
class handleMeg
{
public:
    void recviveMeg(const mulib::net::TcpClient::TcpConnectionPtr &conn, Buffer *buf, mulib::base::Timestamp recviveTime);

private:
    void print(nlohmann::json j);
    void handleNoreadMessage(nlohmann::json &jsonData, MessageManager &megManager_, const mulib::net::TcpClient::TcpConnectionPtr conn);
};
inline void handleMeg::recviveMeg(const mulib::net::TcpClient::TcpConnectionPtr &conn, Buffer *buf, mulib::base::Timestamp recviveTime)
{
    MessageSplitter megSpl;
    megSpl.append(buf);
    std::string msg;
    while (megSpl.nextMessage(msg)){
        LOG_DEBUG << msg;
        auto jsonData = nlohmann::json::parse(msg);
        Type::types type = Type::getDataType(jsonData["type"]);
        LOG_DEBUG << "type:" << type;
        if (type == Type::PRINT)
        {
            print(jsonData);
        }
        else if (type == Type::GETPWD)
        {
            LOG_DEBUG << "进入getpwd";
            if (jsonData["return"] == "email")
            {
                std::cout << "你的账号邮箱为：" << jsonData["email"] << std::endl;
                std::cout << "已发送验证码到你的邮箱" << std::endl;
                jsonData["return"] = "verify";
                conn->send(MessageSplitter::encodeMessage(jsonData.dump()));
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
            auto Copyconn = conn;
            auto ownPtr = std::make_shared<Userui>(userPtr, Copyconn,recviveTime);
            
            std::thread ownuiThread([ownPtr]() { 
                ownPtr->ui(); });
            ownuiThread.detach();
        }
        else if(type == Type::MESSDATA){
            if(jsonData["use"] == "addfriend"){
                std::string allname = jsonData["account"];
                std::string name = allname.substr(5);
                std::cout << "好友申请：" << name << "请求添加你为好友" << std::endl;
            }
        }
        else if(type == Type::SEE){
            if(jsonData["see"] == "friend"){
                std::string name = jsonData["name"];
                std::cout << "好友：" << name.substr(5) << "(" << jsonData["myname"] << ")" << "状态：" << jsonData["mystate"] << "[" << jsonData["degree"] << "条未读]" << std::endl;
            }
        }
        else if(type == Type::MESSAGE){
            LOG_DEBUG << jsonData["from"] << "-" << jsonData["things"];
            if(megManager_.pushMessage(jsonData["from"], jsonData["things"],100)){
                nlohmann::json j;
                j["type"] = "ship";
                j["mystate"] = "offline";
                j["account"] = jsonData["to"];
                conn->send(MessageSplitter::encodeMessage(j.dump()));
                LOG_INFO << "11111";
                handleNoreadMessage(jsonData, megManager_, conn);
                j["mystate"] = "online";
                j["return"] = "1";
                conn->send(MessageSplitter::encodeMessage(j.dump()));
            }
        }
    }
}
inline void handleMeg::handleNoreadMessage(nlohmann::json &jsonData, MessageManager &megManager_, const mulib::net::TcpClient::TcpConnectionPtr conn)
{
    nlohmann::json j;
    std::string userr = jsonData["from"];
    LOG_INFO << userr;
    j["account"] = jsonData["from"];
    j["receive"] = jsonData["to"];
    j["type"] = "message";
    std::queue<std::string> &megQueue = megManager_.fetchMessages(userr);
    if (!megQueue.empty()){
        j["things"] = megQueue.front();
        megQueue.pop();
        LOG_INFO << j.dump();
        conn->send(MessageSplitter::encodeMessage(j.dump()));
    }
}
inline void handleMeg::print(nlohmann::json j)
{
    std::string a = j["meg"];
    if (!a.empty()){
        std::cout << "消息：" << a << std::endl;
    }
    if(j.contains("state")){
        LOG_DEBUG << "state: " << j["state"];
        Type::updataState(j["state"]);
    }
    else if(j.contains("userstate")){
        LOG_DEBUG << "Ustate: " << j["userstate"];
        Type::updataUserState(j["userstate"]);
    }
}
#endif