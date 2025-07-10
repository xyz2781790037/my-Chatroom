#ifndef HANDLEDATA_H
#define HANDLEDATA_H
#include <string>
#include "redisCmd.h"
#include "../netlib/base/logger.h"
#include "../netlib/net/TcpConnection.h"
#include "logOn.h"
namespace mulib{
    namespace net{
        using TcpConnectionPtr = std::shared_ptr<TcpConnection>;
    }
}
class handleData
{
public:
    enum type
    {
        REGISTER,
        LOGIN,
        GETPWD,
        PRINT,
        INFOEMATION
    };
    type getDataType(std::string datatype);
    void handleRegister(const TcpConnectionPtr &conn, nlohmann::json &jsonData, redisCmd &redis);
    void handleLogin(const TcpConnectionPtr &conn, nlohmann::json &jsonData, redisCmd &redis);
    nlohmann::json sendMeg(std::string message, logon::Status state);
};
inline handleData::type handleData::getDataType(std::string datatype){
    if(datatype == "register"){
        return REGISTER;
    }
    else if(datatype == "login"){
        return LOGIN;
    }
    else if(datatype == "getpwd"){
        return GETPWD;
    }
    else if(datatype == "print"){
        return PRINT;
    }
    else if(datatype == "information"){
        return INFOEMATION;
    }
}
inline void handleData::handleRegister(const TcpConnectionPtr &conn, nlohmann::json &jsonData, redisCmd &redis){
    redis.setNewUser(jsonData);
    LOG_INFO << "准备发送注册成功消息给客户端";
    if (conn && conn->connected()){
        conn->send(sendMeg("注册成功！", logon::EXECUTE).dump() + "\n");
        LOG_INFO << "handleData::handleRegister: end";
    }
    else{
        std::cout << "连接尚未建立，无法发送注册信息。\n";
    }
}
inline void handleData::handleLogin(const TcpConnectionPtr &conn, nlohmann::json &jsonData, redisCmd &redis){
    int result = redis.handleLogin(jsonData);
    nlohmann::json jdata;
    if (result == 1)
    {
        jdata["account"] = jsonData["account"];
        redis.returnUser(jdata);
        LOG_INFO << "准备发送 :" << jdata.dump();
        conn->send(jdata.dump() + "\n");
        conn->send(sendMeg("成功登陆", logon::EXECUTE).dump() + "\n");
    }
    else if(result == 0){
        conn->send(sendMeg("密码错误", logon::EXECUTE).dump() + "\n");
    }
    else{
        conn->send(sendMeg("账号不存在",logon::EXECUTE).dump() + "\n");
    }
    LOG_INFO << "handleData::handleLogin: end";
}
inline nlohmann::json handleData::sendMeg(std::string message,logon::Status state){
    nlohmann::json j;
    j["type"] = "print";
    j["meg"] = message;
    j["state"] = state;
    return j;
}
#endif