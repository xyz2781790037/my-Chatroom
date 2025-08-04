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
#include "ConnectionManager.h"
#include "../base/tool.h"
class handleData{
public:
    void Megcycle(const TcpConnectionPtr &conn, MessageSplitter &megser, redisCmd &redis, mulib::base::Timestamp recviveTime);

private:
    void handleRegister(const TcpConnectionPtr &conn, nlohmann::json &jsonData, redisCmd &redis);
    void handleLogin(const TcpConnectionPtr &conn, nlohmann::json &jsonData, redisCmd &redis);
    nlohmann::json sendMeg(std::string message, Type::Status state);
    nlohmann::json sendMeg(std::string message, Type::UserStatus state, std::string N = "n");
    void returnPwd(const TcpConnectionPtr &conn, nlohmann::json &jsonData, redisCmd &redis);
    void revise(const TcpConnectionPtr &conn, nlohmann::json &jsonData, redisCmd &redis);
    void deleteUser(const TcpConnectionPtr &conn, nlohmann::json &jsonData, redisCmd &redis);
    void addAll(const TcpConnectionPtr &conn, nlohmann::json &jsonData, redisCmd &redis);
    void updataShip(const TcpConnectionPtr &conn, nlohmann::json &jsonData, redisCmd &redis);
    void findmess(const TcpConnectionPtr &conn, nlohmann::json &jsonData, redisCmd &redis);
    void verify(const TcpConnectionPtr &conn, nlohmann::json &jsonData, redisCmd &redis);
    void see(const TcpConnectionPtr &conn, nlohmann::json &jsonData, redisCmd &redis);
    void sendOfflineMeg(const TcpConnectionPtr &conn, nlohmann::json &jsonData, redisCmd &redis);
    void handleCmd(const TcpConnectionPtr &conn, nlohmann::json &jsonData, redisCmd &redis);

    std::string code;
};
#endif