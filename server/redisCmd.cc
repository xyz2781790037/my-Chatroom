#include "redisCmd.h"
#include "../netlib/base/logger.h"
#include "../base/logOn.h"
redisCmd::redisCmd(){
    connect();
}
int redisCmd::assignId(){
    int id;
    redisClient.incr("user_id_counter", [&id](cpp_redis::reply &reply)
                     {
    if (reply.is_integer()) {
        id = reply.as_integer();  // 得到新用户的唯一 id
    } });
    redisClient.sync_commit();
    return id;
}
void redisCmd::setNewUser(nlohmann::json &data){
    connect();
    char id[9];
    sprintf(id, "%08d", assignId());
    std::string Id(id);
    std::string key = data["account"];
    std::string password = data["password"];
    std::string qqemail = data["qqEmail"];
    std::string myname = data["myname"];
    redisClient.hset(key, "ID", Id);
    redisClient.hset(key, "password", password);
    redisClient.hset(key, "email", qqemail);
    redisClient.hset(key, "myname", myname);
    redisClient.sync_commit();
}
void redisCmd::connect(){
    if (!redisClient.is_connected()){
        redisClient.connect("127.0.0.1", 6379, [](const std::string &, size_t, cpp_redis::client::connect_state state)
                            {
        if (state == cpp_redis::client::connect_state::ok) {
            std::cout << "[Redis] Connected successfully.\n";
        } else {
            std::cout << "[Redis] Connection failed.\n";
        } });
        redisClient.sync_commit();
    }
}
int redisCmd::handleLogin(nlohmann::json &data){
    connect();
    
    if(!isAccount(data["account"])){
        LOG_INFO << "账号：" << data["account"] << "不存在";
        return -1;
    }
    else{
        if(data["password"] == getPassward(data["account"])){
            LOG_INFO << "账号：" << data["account"] << "存在";
            return 1;
        }
        else{
            LOG_INFO << "密码错误";
            return 0;
        }
    }
}
bool redisCmd::isAccount(std::string account){
    auto reply = redisClient.get(account);
    redisClient.sync_commit();
    auto result = reply.get();
    LOG_INFO << "account is " << result.is_null();
    return !result.is_null();
}
std::string redisCmd::getQQEmail(std::string account){
    return getField(account, "email");
}
std::string redisCmd::getMyname(std::string account){
    return getField(account, "myname");
}
std::string redisCmd::getPassward(std::string account){
    return getField(account, "password");
}
std::string redisCmd::getID(std::string account){
    return getField(account, "ID");
}
std::string redisCmd::getField(const std::string &account, const std::string &field){
    auto reply = redisClient.hget(account, field);
    redisClient.sync_commit();
    auto result = reply.get();
    if (result.is_string()){
        return result.as_string();
    }
    else{
        return "null";
    }
}
void redisCmd::returnUser(nlohmann::json &data){
    std::string account = data["account"];
    data["ID"] = getID(account);
    data["email"] = getQQEmail(account);
    data["myname"] = getMyname(account);
    data["password"] = getPassward(account);
    data["type"] = "information";
}
int redisCmd::Vuser(nlohmann::json &data){
    LOG_INFO << "redisCmd::Vuser";
    if (isAccount(data["account"]))
    {
        if(data["return"] == "email"){
            data["email"] = getQQEmail(data["account"]);
            data["state"] = logon::EXECUTE;
            return 1;
        }
        else if(data["return"] == "verify"){
            data["state"] = logon::EXECUTE;
            return 0;
        }
        else{
            return 2;
        }
    }
    return -1;
}