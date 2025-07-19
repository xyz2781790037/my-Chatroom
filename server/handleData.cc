#include "handleData.h"
ConnectionManager connectionmanger_;
std::unordered_map<std::string, mulib::net::TcpConnectionPtr> findAllConn_;
void handleData::Megcycle(const TcpConnectionPtr &conn, Buffer *buf)
{
    redisCmd redis;
    MessageSplitter megser;
    megser.append(buf);
    std::string jsondata;
    while (megser.nextMessage(jsondata))
    {
        LOG_INFO << jsondata;
        auto jsonData = nlohmann::json::parse(jsondata);
        if (jsonData.contains("type")){
            LOG_DEBUG << "JSON 正常";
            Type::types type = Type::getDataType(jsonData["type"]);
            LOG_INFO << "type is :" << type;
            if (type == Type::REGISTER){
                LOG_INFO << "进入register";
                handleRegister(conn, jsonData, redis);
            }
            else if (type == Type::LOGIN){
                LOG_INFO << "进入login";
                handleLogin(conn, jsonData, redis);
            }
            else if (type == Type::GETPWD){
                LOG_INFO << "进入getpwd";
                returnPwd(conn, jsonData, redis);
                LOG_INFO << "离开getpwd";
            }
            else if (type == Type::REVISE){
                LOG_INFO << "进入revise";
                revise(conn, jsonData, redis);
            }
            else if (type == Type::DELETE){
                LOG_INFO << "进入delete";
                deleteUser(conn, jsonData, redis);
            }
            else if (type == Type::ADD){
                LOG_INFO << "进入add";
                addAll(conn, jsonData, redis);
            }
            else if (type == Type::SHIP){
                LOG_INFO << "进入ship";
                updataShip(conn, jsonData, redis);
            }
            else if (type == Type::MESSDATA){
                LOG_INFO << "进入messdata";
                findmess(conn, jsonData, redis);
            }
            else if (type == Type::VERIFY){
                LOG_INFO << "进入verify";
                verify(conn, jsonData, redis);
            }
            else if (type == Type::SEE){
                LOG_INFO << "进入see";
                see(conn, jsonData, redis);
            }
            else if (type == Type::CHAT){
                sendOfflineMeg(conn, jsonData, redis);
            }
            else if (type == Type::MESSAGE){
                nlohmann::json j;
                j["type"] = "message";
                //解析问题
                j["from"] = jsonData["account"];
                j["to"] = jsonData["receive"];
                j["things"] = jsonData["things"];
                
                if (connectionmanger_.isOnline(jsonData["receive"])){
                    LOG_INFO << jsonData["receive"] << "在线";
                    
                    connectionmanger_.getConn(jsonData["receive"])->send(MessageSplitter::encodeMessage(j.dump()));
                }
                else
                {
                    LOG_INFO << jsonData["receive"] << "不在线";
                    std::string a = j["from"];
                    std::string b = j["to"];
                    redis.storeMessages(b, a, j.dump());
                }
            }
            else if(type == Type::OFFMEG){
                int sum = 0;
                cpp_redis::reply result = redis.see(jsonData);
                const auto &arr = result.as_array();
                for (size_t i = 0; i + 1 < arr.size(); i += 2)
                {
                    int a = std::stoi(arr[i + 1].as_string());
                    sum += a;
                }
                nlohmann::json j;
                std::string meg = "你有" + std::to_string(sum) + "条未读消息";
                LOG_INFO << meg;
                if (sum != 0)
                {
                    conn->send(MessageSplitter::encodeMessage(sendMeg(meg, Type::UEXECUTE).dump()));
                }
                else
                {
                    conn->send(MessageSplitter::encodeMessage(sendMeg("", Type::UEXECUTE).dump()));
                }
            }
        }
        else
        {
            LOG_ERROR << "JSON 数据缺少必要字段，忽略";
        }
    }
}
void handleData::handleRegister(const TcpConnectionPtr &conn, nlohmann::json &jsonData, redisCmd &redis)
{
    if (!redis.isAccount(jsonData["account"]))
    {
        redis.setNewUser(jsonData);
        LOG_INFO << "准备发送注册成功消息给客户端";
        if (conn && conn->connected())
        {
            conn->send(MessageSplitter::encodeMessage(sendMeg("注册成功！", Type::EXECUTE).dump()));
            LOG_INFO << "handleData::handleRegister: end";
        }
        else
        {
            std::cout << "连接尚未建立，无法发送注册信息。\n";
        }
    }
    else
    {
        conn->send(MessageSplitter::encodeMessage(sendMeg("账号已存在", Type::EXECUTE).dump()));
    }
}
void handleData::handleLogin(const TcpConnectionPtr &conn, nlohmann::json &jsonData, redisCmd &redis)
{
    int result = redis.handleLogin(jsonData);
    nlohmann::json jdata;
    if (result == 1)
    {
        jdata["account"] = jsonData["account"];
        redis.returnUser(jdata);
        LOG_INFO << "准备发送 :" << jdata.dump();
        conn->send(MessageSplitter::encodeMessage(sendMeg("成功登陆", Type::UserStatus::UEXECUTE).dump()));
        conn->send(MessageSplitter::encodeMessage(jdata.dump()));
    }
    else if (result == 2)
    {
        conn->send(MessageSplitter::encodeMessage(sendMeg("账号已登录", Type::EXECUTE).dump()));
    }
    else if (result == 0)
    {
        conn->send(MessageSplitter::encodeMessage(sendMeg("密码错误", Type::EXECUTE).dump()));
    }
    else
    {
        conn->send(MessageSplitter::encodeMessage(sendMeg("账号不存在", Type::EXECUTE).dump()));
    }
    LOG_INFO << "handleData::handleLogin: end";
}
nlohmann::json handleData::sendMeg(std::string message, Type::Status state)
{
    nlohmann::json j;
    j["type"] = "print";
    j["meg"] = message;
    j["state"] = state;
    return j;
}
nlohmann::json handleData::sendMeg(std::string message, Type::UserStatus state)
{
    nlohmann::json j;
    j["type"] = "print";
    j["meg"] = message;
    j["userstate"] = state;
    return j;
}
void handleData::returnPwd(const TcpConnectionPtr &conn, nlohmann::json &jsonData, redisCmd &redis)
{
    int result = redis.Vuser(jsonData);
    LOG_INFO << "result is:" << result;
    verCode vercode_;
    if (result == 1)
    {
        conn->send(MessageSplitter::encodeMessage(jsonData.dump()));
    }
    else if (result == 0)
    {
        code = vercode_.verify(redis.getData(jsonData["account"], "email"));
        LOG_INFO << "验证码已发送为：" << code;
        conn->send(MessageSplitter::encodeMessage(jsonData.dump()));
    }
    else if (result == -1)
    {
        LOG_INFO << "result == -1";
        conn->send(MessageSplitter::encodeMessage(sendMeg("账号不存在", Type::RETURN).dump()));
    }
    else if (result == 2)
    {
        if (code == jsonData["vcode"])
        {
            LOG_INFO << "客户验证码正确";
            jsonData["return"] = "true";
            jsonData["state"] = Type::RETURN;
            jsonData["password"] = redis.getData(jsonData["account"], "password");
            conn->send(MessageSplitter::encodeMessage(jsonData.dump()));
        }
        else
        {
            LOG_INFO << "客户验证码错误";
            conn->send(MessageSplitter::encodeMessage(sendMeg("验证码错误", Type::EXECUTE).dump()));
        }
    }
}
void handleData::revise(const TcpConnectionPtr &conn, nlohmann::json &jsonData, redisCmd &redis)
{
    if (jsonData.contains("password"))
    {
        redis.reviseData(jsonData, "password", jsonData["password"]);
    }
    else if (jsonData.contains("myname"))
    {
        redis.reviseData(jsonData, "myname", jsonData["myname"]);
    }
    LOG_INFO << "In handleData::revise";
    std::string a = sendMeg("修改成功！", Type::UEXECUTE).dump();
    LOG_INFO << a;
    conn->send(MessageSplitter::encodeMessage(a));
    LOG_INFO << "conn use count = " << conn.use_count();
}
void handleData::deleteUser(const TcpConnectionPtr &conn, nlohmann::json &jsonData, redisCmd &redis)
{
    redis.deleteUser(jsonData["account"]);
    conn->send(MessageSplitter::encodeMessage(sendMeg("注销成功", Type::EXECUTE).dump()));
}
void handleData::addAll(const TcpConnectionPtr &conn, nlohmann::json &jsonData, redisCmd &redis)
{
    std::string key = jsonData["account"];
    std::string friendname = jsonData["name"];
    std::string lastname = "user:" + friendname;
    int result = 0;
    if (key.substr(0, 5) == "frie:")
    {
        if (redis.isAccount(lastname))
        {
            if (redis.getData(key, lastname) != "null")
            {
                conn->send(MessageSplitter::encodeMessage(sendMeg("他已经是好友了", Type::UEXECUTE).dump()));
            }
            else
            {
                nlohmann::json js;
                js["account"] = jsonData["account"];
                js["type"] = "addfriend";
                js["result"] = "no";
                redis.waitHandleMeg(lastname, js);
                conn->send(MessageSplitter::encodeMessage(sendMeg("好友申请已发送！", Type::UEXECUTE).dump()));
            }
        }
        else
        {
            conn->send(MessageSplitter::encodeMessage(sendMeg("用户不存在！", Type::UEXECUTE).dump()));
        }
    }
}
void handleData::updataShip(const TcpConnectionPtr &conn, nlohmann::json &jsonData, redisCmd &redis)
{
    redis.updataship(jsonData);
    auto Copyconn = conn;
    if(jsonData.contains("return")){
        Copyconn = findAllConn_[jsonData["account"]];
    }
    if (jsonData["mystate"] == "online")
    {
        connectionmanger_.addUserConn(jsonData["account"], Copyconn);
        findAllConn_[jsonData["account"]] = Copyconn;
    }
    else
    {
        LOG_INFO << jsonData["account"];
        connectionmanger_.removeUserConn(jsonData["account"]);
    }
}
void handleData::findmess(const TcpConnectionPtr &conn, nlohmann::json &jsonData, redisCmd &redis)
{
    LOG_INFO << "findmess";
    cpp_redis::reply result = redis.findmess(jsonData);
    for (const auto &item : result.as_array())
    {
        if (item.is_string())
        {
            auto j = nlohmann::json::parse(item.as_string());
            j["use"] = j["type"];
            j["type"] = "messdata";
            LOG_INFO << j.dump();
            conn->send(MessageSplitter::encodeMessage(j.dump()));
        }
    }
    conn->send(MessageSplitter::encodeMessage(sendMeg("--------------------------", Type::URETURN).dump()));
}
void handleData::verify(const TcpConnectionPtr &conn, nlohmann::json &jsonData, redisCmd &redis)
{
    int end = redis.verifyUser(jsonData);
    LOG_INFO << "end= " << end;
    if (end == 1)
    {
        conn->send(MessageSplitter::encodeMessage(sendMeg("你们已经成为好友", Type::URETURN).dump()));
    }
    else if (end == -1)
    {
        conn->send(MessageSplitter::encodeMessage(sendMeg("你已拒绝成为好友", Type::URETURN).dump()));
    }
    else
    {
        conn->send(MessageSplitter::encodeMessage(sendMeg("用户或申请不存在", Type::URETURN).dump()));
    }
}
void handleData::see(const TcpConnectionPtr &conn, nlohmann::json &jsonData, redisCmd &redis)
{
    cpp_redis::reply result = redis.see(jsonData);
    const auto &arr = result.as_array();
    for (size_t i = 0; i + 1 < arr.size(); i += 2)
    {
        nlohmann::json j;
        std::string field = arr[i].as_string();
        std::string value = arr[i + 1].as_string();
        LOG_INFO << "field=" << field << " value=" << value;
        j["type"] = "see";
        j["name"] = field;
        j["myname"] = redis.getData(field, "myname");
        j["see"] = "friend";
        j["mystate"] = redis.getData(field, "mystate");
        j["degree"] = value;
        conn->send(MessageSplitter::encodeMessage(j.dump()));
    }
    conn->send(MessageSplitter::encodeMessage(sendMeg("--------------------------", Type::URETURN).dump()));
}
void handleData::sendOfflineMeg(const TcpConnectionPtr &conn, nlohmann::json &jsonData, redisCmd &redis)
{
    if (redis.isfriend(jsonData["account"], jsonData["name"])){
        redis.sendHistoryMeg(jsonData, conn);
        redis.sendOfflineMeg(jsonData, conn);
        redis.sendHisOffineMeg(jsonData, conn);
        conn->send(MessageSplitter::encodeMessage(sendMeg("开始聊天", Type::UCHAT).dump()));
    }
    else{
        conn->send(MessageSplitter::encodeMessage(sendMeg("用户不存在或好友不存在", Type::URETURN).dump()));
    }
}