#ifndef HANDLEDATA_H
#define HANDLEDATA_H
#include <string>
#include "redisCmd.h"
#include "../netlib/base/logger.h"
#include "../netlib/net/TcpConnection.h"
#include "../base/MegType.h"
#include "../base/MessageSplitter.h"
#include "../base/verCode.h"
#include "../base/logOn.h"
namespace mulib{
    namespace net{
        using TcpConnectionPtr = std::shared_ptr<TcpConnection>;
    }
}
class handleData{
public:
    void Megcycle(const TcpConnectionPtr &conn, Buffer *buf);
private:
    void handleRegister(const TcpConnectionPtr &conn, nlohmann::json &jsonData, redisCmd &redis);
    void handleLogin(const TcpConnectionPtr &conn, nlohmann::json &jsonData, redisCmd &redis);
    nlohmann::json sendMeg(std::string message, Type::Status state);
    nlohmann::json sendMeg(std::string message, Type::UserStatus state);
    void returnPwd(const TcpConnectionPtr &conn, nlohmann::json &jsonData, redisCmd &redis);
    void revise(const TcpConnectionPtr &conn, nlohmann::json &jsonData, redisCmd &redis);
    void deleteUser(const TcpConnectionPtr &conn, nlohmann::json &jsonData, redisCmd &redis);
    void addAll(const TcpConnectionPtr &conn, nlohmann::json &jsonData, redisCmd &redis);
    void updataShip(nlohmann::json &jsonData, redisCmd &redis);

    std::string code;
};
inline void handleData::Megcycle(const TcpConnectionPtr &conn, Buffer *buf)
{
    redisCmd redis;
    MessageSplitter megser;
    megser.append(buf);
    std::string jsondata;
    while (megser.nextMessage(jsondata))
    {
        LOG_INFO << jsondata;
        auto jsonData = nlohmann::json::parse(jsondata);
        if (jsonData.contains("type"))
        {
            LOG_DEBUG << "JSON 正常";
            Type::types type = Type::getDataType(jsonData["type"]);
            LOG_INFO << "type is :" << type;
            if (type == Type::REGISTER)
            {
                LOG_INFO << "进入register";
                handleRegister(conn, jsonData, redis);
            }
            else if (type == Type::LOGIN)
            {
                LOG_INFO << "进入login";
                handleLogin(conn, jsonData, redis);
            }
            else if (type == Type::GETPWD)
            {
                LOG_INFO << "进入getpwd";
                returnPwd(conn, jsonData, redis);
                LOG_INFO << "离开getpwd";
            }
            else if(type == Type::REVISE){
                LOG_INFO << "进入revise";
                revise(conn, jsonData, redis);
            }
            else if(type == Type::DELETE){
                LOG_INFO << "进入delete";
                deleteUser(conn, jsonData, redis);
            }
            else if(type == Type::ADD){
                LOG_INFO << "进入add";
                addAll(conn,jsonData,redis);
            }
            else if(type == Type::SHIP){
                updataShip(jsonData, redis);
            }
        }
        else
        {
            LOG_ERROR << "JSON 数据缺少必要字段，忽略";
        }
    }
}
inline void handleData::handleRegister(const TcpConnectionPtr &conn, nlohmann::json &jsonData, redisCmd &redis){
    if(!redis.isAccount(jsonData["account"])){
        redis.setNewUser(jsonData);
        LOG_INFO << "准备发送注册成功消息给客户端";
        if (conn && conn->connected()){
            conn->send(sendMeg("注册成功！", Type::EXECUTE).dump() + "\n");
            LOG_INFO << "handleData::handleRegister: end";
        }
        else{
            std::cout << "连接尚未建立，无法发送注册信息。\n";
        }
    }
    else{
        conn->send(sendMeg("账号已存在",Type::EXECUTE).dump() + "\n");
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
        conn->send(sendMeg("成功登陆", Type::UserStatus::UEXECUTE).dump() + "\n");
        conn->send(jdata.dump() + "\n");
        
    }
    else if(result == 0){
        conn->send(sendMeg("密码错误", Type::EXECUTE).dump() + "\n");
    }
    else{
        conn->send(sendMeg("账号不存在",Type::EXECUTE).dump() + "\n");
    }
    LOG_INFO << "handleData::handleLogin: end";
}
inline nlohmann::json handleData::sendMeg(std::string message,Type::Status state){
    nlohmann::json j;
    j["type"] = "print";
    j["meg"] = message;
    j["state"] = state;
    return j;
}
inline nlohmann::json handleData::sendMeg(std::string message, Type::UserStatus state)
{
    nlohmann::json j;
    j["type"] = "print";
    j["meg"] = message;
    j["userstate"] = state;
    return j;
}
inline void handleData::returnPwd(const TcpConnectionPtr &conn, nlohmann::json &jsonData, redisCmd &redis){
    int result = redis.Vuser(jsonData);
    LOG_INFO << "result is:" << result;
    verCode vercode_;
    if (result == 1)
    {
        conn->send(jsonData.dump() + "\n");
    }
    else if(result == 0){
        code = vercode_.verify(redis.getQQEmail(jsonData["account"]));
        LOG_INFO << "验证码已发送为：" << code;
        conn->send(jsonData.dump() + "\n");
    }
    else if(result == -1){
        LOG_INFO << "result == -1";
        conn->send(sendMeg("账号不存在", Type::RETURN).dump() + "\n");
    }
    else if(result == 2){
        if (code == jsonData["vcode"])
        {
            LOG_INFO << "客户验证码正确";
            jsonData["return"] = "true";
            jsonData["state"] = Type::RETURN;
            jsonData["password"] = redis.getPassward(jsonData["account"]);
            conn->send(jsonData.dump() + "\n");
        }
        else{
            LOG_INFO << "客户验证码错误";
            conn->send(sendMeg("验证码错误", Type::EXECUTE).dump() + "\n");
        }
    }
}
inline void handleData::revise(const TcpConnectionPtr &conn, nlohmann::json &jsonData, redisCmd &redis){
    if(jsonData.contains("password")){
        redis.reviseData(jsonData,"password",jsonData["password"]);
    }
    else if(jsonData.contains("myname")){
        redis.reviseData(jsonData, "myname", jsonData["myname"]);
    }
    LOG_INFO << "In handleData::revise";
    std::string a = sendMeg("修改成功！", Type::UEXECUTE).dump() + "\n";
    LOG_INFO << a;
    conn->send(a);
    LOG_INFO << "conn use count = " << conn.use_count();
}
inline void handleData::deleteUser(const TcpConnectionPtr &conn, nlohmann::json &jsonData, redisCmd &redis){
    redis.deleteUser(jsonData["account"]);
    conn->send(sendMeg("注销成功", Type::EXECUTE).dump() + "\n");
}
inline void handleData::addAll(const TcpConnectionPtr &conn, nlohmann::json &jsonData, redisCmd &redis){
    std::string key = jsonData["account"];
    std::string friendname = jsonData["name"];
    int result = 0;
    if (key.substr(0, 5) == "fire:"){
        result = redis.addFriend(key,"user:" + friendname);
    }
    if(result == -1){
        conn->send(sendMeg("用户不存在！", Type::UEXECUTE).dump() + "\n");
    }
    else if(result == -2){
        
    }
}
inline void handleData::updataShip(nlohmann::json &jsonData, redisCmd &redis){
    redis.updataship(jsonData);
}
#endif