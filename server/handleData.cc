#include "handleData.h"
ConnectionManager connectionmanger_;
void handleData::Megcycle(const TcpConnectionPtr &conn, Buffer *buf){
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
                updataShip(conn,jsonData, redis);
            }
            else if(type == Type::MESSDATA){
                LOG_INFO << "进入messdata";
                findmess(conn,jsonData,redis);
            }
            else if(type == Type::VERIFY){
                verify(conn, jsonData, redis);
            }
        }
        else
        {
            LOG_ERROR << "JSON 数据缺少必要字段，忽略";
        }
    }
}
void handleData::handleRegister(const TcpConnectionPtr &conn, nlohmann::json &jsonData, redisCmd &redis){
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
void handleData::handleLogin(const TcpConnectionPtr &conn, nlohmann::json &jsonData, redisCmd &redis){
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
    else if(result == 2){
        conn->send(sendMeg("账号已登录", Type::EXECUTE).dump() + "\n");
    }
    else if(result == 0){
        conn->send(sendMeg("密码错误", Type::EXECUTE).dump() + "\n");
    }
    else{
        conn->send(sendMeg("账号不存在",Type::EXECUTE).dump() + "\n");
    }
    LOG_INFO << "handleData::handleLogin: end";
}
nlohmann::json handleData::sendMeg(std::string message,Type::Status state){
    nlohmann::json j;
    j["type"] = "print";
    j["meg"] = message;
    j["state"] = state;
    return j;
}
nlohmann::json handleData::sendMeg(std::string message, Type::UserStatus state){
    nlohmann::json j;
    j["type"] = "print";
    j["meg"] = message;
    j["userstate"] = state;
    return j;
}
void handleData::returnPwd(const TcpConnectionPtr &conn, nlohmann::json &jsonData, redisCmd &redis){
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
void handleData::revise(const TcpConnectionPtr &conn, nlohmann::json &jsonData, redisCmd &redis){
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
void handleData::deleteUser(const TcpConnectionPtr &conn, nlohmann::json &jsonData, redisCmd &redis){
    redis.deleteUser(jsonData["account"]);
    conn->send(sendMeg("注销成功", Type::EXECUTE).dump() + "\n");
}
void handleData::addAll(const TcpConnectionPtr &conn, nlohmann::json &jsonData, redisCmd &redis){
    std::string key = jsonData["account"];
    std::string friendname = jsonData["name"];
    std::string lastname = "user:" + friendname;
    int result = 0;
    if (key.substr(0, 5) == "fire:"){
        if(redis.isAccount(lastname)){
            if(jsonData.contains(lastname)){
                conn->send(sendMeg("他已经是好友了", Type::UEXECUTE).dump() + "\n");
            }
            else{
                nlohmann::json js;
                js["account"] = jsonData["account"];
                js["type"] = "addfriend";
                js["result"] = "no";
                redis.waitHandleMeg(lastname,js);
                conn->send(sendMeg("好友申请已发送！", Type::UEXECUTE).dump() + "\n");
            }
        }
        else{
            conn->send(sendMeg("用户不存在！", Type::UEXECUTE).dump() + "\n");
        }
    }
}
void handleData::updataShip(const TcpConnectionPtr &conn, nlohmann::json &jsonData, redisCmd &redis){
    redis.updataship(jsonData);
    auto Copyconn = conn;
    if(jsonData["mystate"] == "online"){
        connectionmanger_.addUserConn(jsonData["account"], Copyconn);
    }
    else{
        connectionmanger_.removeUserConn(jsonData["account"]);
    }
}
void handleData::findmess(const TcpConnectionPtr &conn, nlohmann::json &jsonData, redisCmd &redis){
    LOG_INFO << "findmess";
    cpp_redis::reply result = redis.findmess(jsonData);
    for (const auto &item : result.as_array()){
        if (item.is_string()){
            auto j = nlohmann::json::parse(item.as_string());
            j["use"] = j["type"];
            j["type"] = "messdata";
            LOG_INFO << j.dump();
            conn->send(j.dump() + "\n");
        }
    }
    conn->send(sendMeg("--------------------------", Type::URETURN).dump() + "\n");
}
void handleData::verify(const TcpConnectionPtr &conn, nlohmann::json &jsonData, redisCmd &redis){
    std::string a = jsonData["name"];
    if(jsonData["result"] == "yes" && redis.isAccount("user:" + a)){
        redis.addFriend(jsonData["account"], jsonData["name"]);
        redis.addFriend(jsonData["name"], jsonData["account"]);
        conn->send(sendMeg("你们已经成为好友", Type::URETURN).dump() + "\n");
    }
    else if(jsonData["result"] == "no"){
        conn->send(sendMeg("你已拒绝成为好友", Type::URETURN).dump() + "\n");
    }
    else{
        conn->send(sendMeg("用户不存在", Type::URETURN).dump() + "\n");
    }
}