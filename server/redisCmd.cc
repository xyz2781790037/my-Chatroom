#include "redisCmd.h"
#include "../netlib/base/logger.h"
#include "../base/logOn.h"
#include "../base/tool.h"
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
void redisCmd::reviseData(std::string key, std::string type,std::string typedata){
    connect();
    LOG_INFO << "key= " << key;
    redisClient.hset(key, type, typedata);
    redisClient.sync_commit();
}
void redisCmd::deleteUser(std::string account){
    connect();
    std::string key = account;
    redisClient.del({key});
    redisClient.del({"mess:" + key});
    redisClient.del({"offl:" + key.substr(5)});
    auto reply = redisClient.hgetall("frie:" + account.substr(5));
    redisClient.sync_commit();
    auto result = reply.get();
    const auto&arr = result.as_array();
    for (int i = 0; i + 1 < arr.size();i+=2){
        std::string name = "frie:" + arr[i].as_string().substr(5);
        redisClient.hdel(name, {account});
        std::string history = tool::swapsort(account,name,"read:");
        redisClient.del({history});
        redisClient.sync_commit();
    }
    redisClient.del({"frie:" + key.substr(5)});
    redisClient.sync_commit();
    std::string keyy = "mygp:" + key.substr(5);
    auto reply1 = redisClient.hgetall("mygp:" + account.substr(5));
    redisClient.sync_commit();
    auto result1 = reply1.get();
    const auto &str = result1.as_array();
    for (int i = 0; i + 1 < str.size();i+=2){
        std::string name = "grop:" + str[i].as_string();
        if(getData(name,key) == "owner"){
            redisClient.del({name});
        }
        else{
            redisClient.hdel(name, {key});
        }
        redisClient.sync_commit();
    }
    redisClient.del({keyy});
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
    redisClient.hset(account1, name, "0");
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
cpp_redis::reply redisCmd::findmess(std::string name){
    connect();
    LOG_INFO << name;
    auto reply = redisClient.lrange(name, 0, -1);
    redisClient.sync_commit();
    auto result = reply.get();
    if(result.is_array()){
        return result;
    }
    return cpp_redis::reply();
}
int redisCmd::verifyUser(nlohmann::json &data){
    connect();
    std::string dataa, last;
    if(data["pass"] == "addfriend" || data["pass"] == "invitation"){
        dataa = data["account"];
        last = "mess:user:" + dataa.substr(5);
    }
    else if (data["pass"] == "addgroup")
    {
        dataa = data["group"];
        last = "mess:grop:" + dataa;
    }
    auto reply = redisClient.lrange(last, 0, -1);
    redisClient.sync_commit();
    auto result = reply.get();
    for (const auto &item : result.as_array())
    {
        auto j = nlohmann::json::parse(item.as_string());
        std::string a = data["name"];
        LOG_INFO << "a=" + a;
        if (j["account"] == "frie:" + a && j["type"] == "addfriend"){
            if(data["result"] == "yes"){
                addFriend(dataa.substr(5), data["name"]);
                addFriend(data["name"], dataa.substr(5));
                redisClient.lrem(last, 0, item.as_string());
                redisClient.sync_commit();
                return 1;
            }
            else{
                // 缺少函数
            }
        }
        else if(j["account"] == "mygp:" + a && j["type"] == "addgroup"){
            if(data["result"] == "yes"){
                reviseData(j["account"], data["group"], "0");
                reviseData("grop:" + dataa, "user:" + a, "member");
                redisClient.lrem(last, 0, item.as_string());
                redisClient.sync_commit();
                return 2;
            }
            else{
                redisClient.lrem(last, 0, item.as_string());
                redisClient.sync_commit();
                return -2;
            }
        }
        else if(j["name"] == "grop:" + a && j["type"] == "invitation"){
            if (data["result"] == "yes")
            {
                reviseData(dataa, data["name"], "0");
                reviseData("grop:" + a, "user:" + dataa.substr(5), "member");
                redisClient.lrem(last, 0, item.as_string());
                redisClient.sync_commit();
                return 2;
            }
            else
            {
                redisClient.lrem(last, 0, item.as_string());
                redisClient.sync_commit();
                return -2;
            }
        }
    }
    return 0;
}
int redisCmd::see(std::string data){
    connect();
    auto reply = redisClient.hgetall(data);
    redisClient.sync_commit();
    auto result = reply.get();
    int sum = 0;
    if (result.is_array()){
        const auto &arr = result.as_array();
        for (size_t i = 0; i + 1 < arr.size(); i += 2){
            int a = std::stoi(arr[i + 1].as_string());
            sum += a;
        }
    }
    return sum;
}
bool redisCmd::isfriend(std::string account, std::string name){
    std::string key = account.substr(5);
    std::string field = name.substr(5);
    return getData("frie:" + key, "user:" + field) != "null";
}
void redisCmd::storeMessages(std::string sender,std::string account,std::string message)
{
    connect();
    LOG_INFO << "storeMessage";
    connect();
    LOG_INFO << "存储新消息：" << message;
    
    std::string key = "offl:" + sender.substr(5);
    LOG_INFO << key;
    LOG_INFO << sender << " " << account;
    int count = getRedisCount("frie:" + sender.substr(5), account);
    redisClient.hset(key, account + std::to_string(count++), message);
    redisClient.sync_commit();
    LOG_INFO << "count=" << count;
    redisClient.hset("frie:" + sender.substr(5), account, std::to_string(count));
    redisClient.sync_commit();
}
void redisCmd::sendOfflineMeg(nlohmann::json &data,const TcpConnectionPtr &conn){
    connect();
    std::string key = data["account"];
    std::string field = data["name"];
    LOG_INFO << "key=" << key << " field=" << field;
    int count = getRedisCount(key, field);
    redisClient.hset(key, field, "0");
    LOG_INFO << "count=" << count;
    for (int i = 0; i < count;i++){
        std::string mesg = getRedisResult("offl:" + key.substr(5), field + std::to_string(i)).as_string();
        conn->send(MessageSplitter::encodeMessage(mesg));
        std::string key1 = tool::swapsort(key, field, "read:");
        storeReadMeg(key, field, mesg,key1);
        LOG_INFO << "删除";
        redisClient.hdel("offl:" + key.substr(5), {field + std::to_string(i)});
        redisClient.sync_commit();
    }
}
int redisCmd::getRedisCount(std::string key,std::string field){
    if (getRedisResult(key, field) != cpp_redis::reply()){
        return std::stoi(getRedisResult(key, field).as_string());
    }
    return 0;
}
cpp_redis::reply redisCmd::getRedisResult(std::string key, std::string field){
    auto reply = redisClient.hget(key, field);
    redisClient.sync_commit();
    auto result = reply.get();
    if(result.is_string()){
        return result;
    }
    return cpp_redis::reply();
}
void redisCmd::storeReadMeg(std::string sender, std::string account, std::string message,std::string key){
    LOG_INFO << "存储新消息：" << message;
    std::vector<std::string> v = {message};
    redisClient.rpush(key, v);
    redisClient.sync_commit();
}
void redisCmd::sendHistoryMeg(nlohmann::json &data, const TcpConnectionPtr &conn){
    int count = 0;
    std::string a = data["account"];
    std::string b = data["name"];
    std::string key = tool::swapsort(a, b , "read:");
    LOG_INFO << "历史消息key=" << key;
    auto reply = redisClient.lrange(key, 0, -1);
    redisClient.sync_commit();
    auto result = reply.get();
    for(const auto &item: result.as_array()){
        auto meg = item.as_string();
        LOG_INFO << "历史消息：" << meg;
        conn->send(MessageSplitter::encodeMessage(item.as_string()));
        LOG_INFO << count++;
    }
}
void redisCmd::sendHisOffineMeg(nlohmann::json &data, const TcpConnectionPtr &conn){
    std::string key = data["name"];
    std::string field = data["account"];
    LOG_INFO << "key=" << key << " field=" << field;
    int count = getRedisCount("frie:" + key.substr(5), "user:" + field.substr(5));
    LOG_INFO << "count=" << count;
    for (int i = 0; i < count;i++){
        std::string mesg = getRedisResult("offl:" + key.substr(5), "user:" + field.substr(5) + std::to_string(i)).as_string();
        LOG_INFO << "our send:" << mesg;
        conn->send(MessageSplitter::encodeMessage(mesg));
    }
}
void redisCmd::delFriend(std::string user, std::string name){
    std::vector<std::string> field = {name};
    std::vector<std::string> field2 = {"user:" + user.substr(5)};
    LOG_INFO << "will del user=" << user << " name=" << name;
    redisClient.hdel(user, field);
    redisClient.hdel("frie:" + name.substr(5), field2);
    redisClient.sync_commit();
}
void redisCmd::black(std::string user, std::string name){
    std::string field = "blak:" + user.substr(5);
    redisClient.hset(field, name, "0");
    redisClient.sync_commit();
}
void redisCmd::sendBlackMeg(nlohmann::json &data, const TcpConnectionPtr &conn){
    std::string a = data["account"];
    std::string b = data["name"];
    std::string key = tool::spellName(a, b, "back:");
    LOG_INFO << "black消息key=" << key;
    auto reply = redisClient.lrange(key, 0, -1);
    redisClient.sync_commit();
    auto result = reply.get();
    for (const auto &item : result.as_array()){
        auto meg = item.as_string();
        LOG_INFO << "历史消息：" << meg;
        conn->send(MessageSplitter::encodeMessage(item.as_string()));
        // std::this_thread::sleep_for(std::chrono::microseconds(300));
    }
}
void redisCmd::joinGroup(std::string key,std::string account,std::string rank){
    if (isAccount("user:" + account.substr(5))){
        redisClient.hset(key, account, rank);
        redisClient.hset("mygp:" + account.substr(5), key.substr(5), "0");
        redisClient.sync_commit();
    }
}
void redisCmd::createGroup(nlohmann::json &data){
    std::string count1 = data["count"];
    std::string name = data["name"];
    int count = std::stoi(count1);
    std::string key = "grop:" + name;
    joinGroup(key, data["account"], "owner");
    for (int i = 0; i < count; i++){
        joinGroup(key, data["member" + std::to_string(i)], "member");
    }
}
bool redisCmd::isgroup(std::string name){
    connect();
    auto reply = redisClient.get("grop:" + name);
    redisClient.sync_commit();
    auto result = reply.get();
    LOG_INFO << "account is " << result.is_null();
    return !result.is_null();
}
void redisCmd::delPerson(std::string name, std::string account,std::string rank){
    if(rank == "owner"){
        auto reply = redisClient.hgetall("grop:" + name);
        redisClient.sync_commit();
        auto result = reply.get();
        if (result.is_array()){
            const auto &arr = result.as_array();
            for (size_t i = 0; i + 1 < arr.size(); i += 2){
                std::string key = arr[i].as_string();
                redisClient.hdel("mygp:" + key.substr(5), {name});
            }
        }
        redisClient.del({"grop:" + name});
        redisClient.del({"regp:" + name});
    }else{
        redisClient.hdel("grop:" + name, {account});
        redisClient.hdel("mygp:" + account.substr(5), {name});
    }
    redisClient.sync_commit();
}
cpp_redis::reply redisCmd::seeGroup(std::string key){
    LOG_INFO << "redisCmd::seeGroup";
    auto reply = redisClient.hgetall(key);
    redisClient.sync_commit();
    auto result = reply.get();
    if (result.is_array()){
        LOG_INFO << "seeGroup 正常退出";
        return result;
    }
    return cpp_redis::reply();
}
bool redisCmd::ismygroup(std::string name, std::string account){
    return getData(account, name.substr(5)) != "null";
}
void redisCmd::storeGroupMeg(nlohmann::json &data, std::string msg){
    std::string name = data["group"];
    std::string sender = data["account"];
    LOG_INFO << "redisCmd::offline group message";
    redisClient.rpush("regp:" + name.substr(5), {msg});
    redisClient.sync_commit();
}
void redisCmd::getGroupMeg(std::string key, std::string name, const TcpConnectionPtr &conn){
    int count = 0;
    std::string nkey = "regp:" + key.substr(5);
    auto reply = redisClient.lrange(nkey, 0, -1);
    redisClient.sync_commit();
    auto result = reply.get();
    for(const auto& item : result.as_array()){
        conn->send(MessageSplitter::encodeMessage(item.as_string()));
        LOG_INFO << ++count;
    }
    redisClient.hset(name, key.substr(5), "0");
    redisClient.sync_commit();
}
void redisCmd::hset(std::string key, std::string field, std::string count){
    redisClient.hset(key,field, count);
    redisClient.sync_commit();
}
void redisCmd::hdel(std::string key, std::string field){
    redisClient.hdel(key, {field});
    redisClient.sync_commit();
}
std::string redisCmd::getLevel(std::string groupname, std::string username){
    std::string key, field;
    if (groupname.size() <= 5){
        key = "grop:" + groupname;
    }
    else{
        key = "grop:" + groupname.substr(5);
    }
    if(username.size() <= 5){
        field = "user:" + username;
    }
    else{
        field = "user:" + username.substr(5);
    }
    LOG_INFO << "key=" << key << " field=" << field;
    return getData(key, field);
}
void redisCmd::delmember(std::string group, std::string user){
    redisClient.hdel(group,{user});
    redisClient.hdel("mygp:" + user.substr(5), {group.substr(5)});
    redisClient.sync_commit();
}
void redisCmd::hpush(std::string key,std::string meg){
    redisClient.rpush(key, {meg});
    redisClient.sync_commit();
}
bool redisCmd::lookmess(std::string key, nlohmann::json &data){
    auto reply = redisClient.lrange(key,0 ,-1);
    redisClient.sync_commit();
    auto result = reply.get();
    if(result.is_array()){
        for(const auto&item : result.as_array()){
            if(item.as_string() == data.dump()){
                LOG_INFO << item.as_string();
                std::string name = data["account"];
                hset(name, key.substr(5), "0");
                hset("frie:" + key.substr(10), "user:" + name.substr(5), "0");
                redisClient.lrem(key, 0, item.as_string());
                redisClient.sync_commit();
                return false;
            }
        }
    }
    return true;
}
int redisCmd::getVerifyLen(std::string key){
    LOG_INFO << key;
    auto reply = redisClient.llen(key);
    redisClient.sync_commit();
    auto result = reply.get();
    if(result.is_integer()){
        return result.as_integer();
    }
    return 0;
}