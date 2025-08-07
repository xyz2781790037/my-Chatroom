#ifndef HANDERMESSAGE_H
#define HANDERMESSAGE_H
#include "../base/MessageSplitter.h"
#include <../netlib/net/TcpClient.h>
#include "../netlib/base/logger.h"
#include <nlohmann/json.hpp>
#include "../base/MegType.h"
#include "../base/user.h"
#include "UserUi.h"
#include <atomic>
class handleMeg
{
public:
    void recviveMeg(const mulib::net::TcpClient::TcpConnectionPtr &conn, mulib::base::Timestamp recviveTime, std::string msg);
    void getConn(const mulib::net::TcpClient::TcpConnectionPtr conn);

private:
    void print(nlohmann::json j);
    void handleNoreadMessage(nlohmann::json &jsonData, const mulib::net::TcpClient::TcpConnectionPtr conn);
    std::shared_ptr<mulib::net::TcpConnection> ftpConn;
};
inline void handleMeg::getConn(const mulib::net::TcpClient::TcpConnectionPtr conn){
    ftpConn = conn;
}
inline void handleMeg::recviveMeg(const mulib::net::TcpClient::TcpConnectionPtr &conn, mulib::base::Timestamp recviveTime, std::string msg){
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
            auto ownPtr = std::make_shared<Userui>(userPtr, Copyconn,ftpConn,recviveTime);
            
            std::thread ownuiThread([ownPtr]() { 
                ownPtr->ui();
             });
            ownuiThread.detach();
        }
        else if(type == Type::MESSDATA){
            if(jsonData["use"] == "addfriend"){
                std::string allname = jsonData["account"];
                std::string name = allname.substr(5);
                std::cout << "好友申请：" << name << "请求添加你为好友" << std::endl;
            }
            else if(jsonData["use"] == "addgroup"){
                std::string allname = jsonData["account"];
                std::string name = allname.substr(5);
                std::cout << "加群申请：" << name << "请求进入" << jsonData["name"] << std::endl;
            }
            else if(jsonData["use"] == "invitation"){
                std::string allname = jsonData["account"];
                std::string gp = jsonData["name"];
                std::string name = allname.substr(5);
                char c = '"';
                std::cout << "邀请信息：" << name << "邀请你加入" << c << gp.substr(5) << c << std::endl;
            }
        }
        else if(type == Type::SEE){
            if(jsonData["see"] == "friend"){
                std::string name = jsonData["name"];
                printf("好友：%s(%s)状态：%s[%s条未读]\n", name.substr(5).c_str(), jsonData["myname"].dump().c_str(), jsonData["mystate"].dump().c_str(), jsonData["degree"].dump().c_str());
            }
            else if(jsonData["see"] == "black"){
                std::string name = jsonData["name"];
                std::cout << "好友:" << name << "\033[1;35m(已拉黑)\033[0m" << std::endl;
            }
        }
        else if(type == Type::MESSAGE){
            LOG_DEBUG << "\033[1;35m" << std::this_thread::get_id() << "\033[0m";
            LOG_DEBUG << jsonData["from"] << "-" << jsonData["things"];
            std::string key = tool::swapsort(jsonData["from"],jsonData["to"],"read:");
            if(megManager_.pushMessage(key, jsonData["things"],3000000))
                ;
            messageReminder = true;
            chatCv.notify_one();
        }
        else if(type == Type::SEEGROUP){
            std::cout << "群聊：" << jsonData["name"] << "[" << jsonData["amount"] << "条消息]" << std::endl; 
        }
        else if (type == Type::GMESSAGE){
            LOG_DEBUG << jsonData["from"] << "-" << jsonData["things"];
            std::string key = jsonData["from"];
            LOG_DEBUG << "\033[1;35m" << std::this_thread::get_id() << "\033[0m";
            if (groupMegManager_.pushMessage(key, jsonData["things"], 3000000))
                ;
            gmessageReminder = true;
            gchatCv.notify_one();
        }
    
}
inline void handleMeg::handleNoreadMessage(nlohmann::json &jsonData, const mulib::net::TcpClient::TcpConnectionPtr conn){
    nlohmann::json j;
    j["account"] = jsonData["from"];
    j["receive"] = jsonData["to"];
    j["type"] = "message";
    j["things"] = jsonData["things"];
    // LOG_INFO << j.dump();
    conn->send(MessageSplitter::encodeMessage(j.dump()));
}
inline void handleMeg::print(nlohmann::json j)
{
    std::string a = j["meg"];
    if (!a.empty()){
        if(j["n"] == "n"){
            std::cout << "系统：" << a << std::endl;
            
        }
        else{
            std::cout << "系统：" << a;
            std::cout << "发送:" << std::flush;
        }
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