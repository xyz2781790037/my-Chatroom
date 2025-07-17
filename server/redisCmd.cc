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
        if(data["password"] == getData(data["account"],"password")){
            LOG_INFO << "账号：" << data["account"] << "存在";
            if (getData(data["account"],"mystate") == "online"){
                LOG_WARN << "账号已登陆";
                return 2;
            }
            return 1;
        }
        else{
            LOG_INFO << "密码错误";
            return 0;
        }
    }
}
bool redisCmd::isAccount(std::string account){
    connect();
    auto reply = redisClient.get(account);
    redisClient.sync_commit();
    auto result = reply.get();
    LOG_INFO << "account is " << result.is_null();
    return !result.is_null();
}
std::string redisCmd::getData(std::string account,std::string dataname){
    return getField(account, dataname);
}
std::string redisCmd::getField(const std::string &account, const std::string &field){
    connect();
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
    connect();
    std::string account = data["account"];
    data["ID"] = getData(account,"ID");
    data["email"] = getData(account,"email");
    data["myname"] = getData(account,"myname");
    data["password"] = getData(account,"password");
    data["type"] = "information";
}
int redisCmd::Vuser(nlohmann::json &data){
    connect();
    LOG_INFO << "redisCmd::Vuser";
    if (isAccount(data["account"]))
    {
        if(data["return"] == "email"){
            data["email"] = getData(data["account"],"email");
            data["state"] = Type::EXECUTE;
            return 1;
        }
        else if(data["return"] == "verify"){
            data["state"] = Type::EXECUTE;
            return 0;
        }
        else{
            return 2;
        }
    }
    return -1;
}
void redisCmd::reviseData(nlohmann::json &data, std::string type,std::string typedata){
    connect();
    std::string key = data["account"];
    LOG_INFO << "key= " << key;
    redisClient.hset(key, type, typedata);
    redisClient.sync_commit();
}
void redisCmd::deleteUser(std::string account){
    connect();
    std::vector<std::string> key = {account};
    redisClient.del(key);
    redisClient.sync_commit();
}
void redisCmd::updataship(nlohmann::json &data){
    connect();
    std::string key = data["account"];
    std::string mystate = data["mystate"];
    redisClient.hset(key, "mystate", mystate);
    redisClient.sync_commit();
}
void redisCmd::addFriend(std::string account,std::string friendname){
    connect();
    std::string account1 = "frie:" + account;
    std::string name = "user:" + friendname;
    redisClient.hset(account1, name, "ordinary");
    redisClient.sync_commit();
}
void redisCmd::waitHandleMeg(std::string Key,nlohmann::json &data){
    connect();
    std::string key = "mess:" + Key;
    std::string mess = data.dump();
    std::vector<std::string> a = {mess};
    redisClient.lpush(key,a);
    redisClient.sync_commit();
}
cpp_redis::reply redisCmd::findmess(nlohmann::json &data)
{
    connect();
    LOG_INFO << data["account"];
    auto reply = redisClient.lrange(data["account"], 0, -1);
    redisClient.sync_commit();
    auto result = reply.get();
    if(result.is_array()){
        return result;
    }
    return cpp_redis::reply();
}
int redisCmd::verifyUser(nlohmann::json &data){
    std::string dataa = data["account"];
    std::string last = "mess:user:" + dataa;
    auto reply = redisClient.lrange(last, 0, -1);
    redisClient.sync_commit();
    auto result = reply.get();
    for (const auto &item : result.as_array())
    {
        auto j = nlohmann::json::parse(item.as_string());
        std::string a = data["name"];
        LOG_INFO << "frie:" + a;
        if (j["account"] == "frie:" + a && j["type"] == "addfriend"){
            if(data["result"] == "yes"){
                addFriend(data["account"], data["name"]);
                addFriend(data["name"], data["account"]);
                redisClient.lrem(last, 0, item.as_string());
                redisClient.sync_commit();
                return 1;
            }
            else{
                redisClient.lrem(last, 0, item.as_string());
                redisClient.sync_commit();
                return -1;
            }
        }
    }
    return 0;
}
cpp_redis::reply redisCmd::see(nlohmann::json &data){
    auto reply = redisClient.hgetall(data["account"]);
    redisClient.sync_commit();
    auto result = reply.get();
    if (result.is_array())
    {
        return result;
    }
    return cpp_redis::reply();
}
bool redisCmd::isfriend(std::string account, std::string name){
    return getData(account, name) != "null";
}