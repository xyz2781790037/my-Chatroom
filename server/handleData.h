#ifndef HANDLEDATA_H
#define HANDLEDATA_H
#include <string>
#include "redisCmd.h"
#include "../netlib/base/logger.h"
#include "../netlib/net/TcpConnection.h"
#include "../base/MegType.h"
#include "../base/MessageSplitter.h"
#include "../base/verCode.h"
#include "ConnectionManager.h"
#include "../base/tool.h"
#include <unordered_map>
class handleData
{
public:
    void Megcycle(const TcpConnectionPtr conn, std::string &meg, redisCmd &redis, mulib::base::Timestamp recviveTime);

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
    void offMeg(const TcpConnectionPtr &conn, nlohmann::json &jsonData, redisCmd &redis);
    void endChat(const TcpConnectionPtr &conn, nlohmann::json &jsonData, redisCmd &redis);
    void deleteEnd(const TcpConnectionPtr &conn, nlohmann::json &jsonData, redisCmd &redis);
    void balckName(const TcpConnectionPtr &conn, nlohmann::json &jsonData, redisCmd &redis);
    void createGroup(const TcpConnectionPtr &conn, nlohmann::json &jsonData, redisCmd &redis);
    void deleteGroup(const TcpConnectionPtr &conn, nlohmann::json &jsonData, redisCmd &redis);
    void seeGroup(const TcpConnectionPtr &conn, nlohmann::json &jsonData, redisCmd &redis);
    void gChat(const TcpConnectionPtr &conn, nlohmann::json &jsonData, redisCmd &redis);
    void handlegMessage(const TcpConnectionPtr &conn, nlohmann::json &jsonData, redisCmd &redis);
    void handleFile(const TcpConnectionPtr &conn, nlohmann::json &jsonData, redisCmd &redis);
    void lookFile(const TcpConnectionPtr &conn, nlohmann::json &jsonData, redisCmd &redis);
    void list(const TcpConnectionPtr &conn, nlohmann::json &jsonData, redisCmd &redis);
    void seeHistory(const TcpConnectionPtr &conn, nlohmann::json &jsonData, redisCmd &redis);
    void heartTcp(const TcpConnectionPtr &conn, nlohmann::json &jsonData, redisCmd &redis, mulib::base::Timestamp recviveTime);

    std::unordered_map<mulib::net::TcpConnectionPtr, std::string> codes_;
};
#endif