#ifndef REDISCMD_H
#define REDISCMD_H
#include <cpp_redis/cpp_redis>
#include <nlohmann/json.hpp>
#include "../netlib/net/TcpServer.h"
extern std::mutex storeMtx;
extern std::mutex storeMtx2;
extern std::unordered_map<std::string, std::vector<std::string>> messageQueue;
extern std::map<std::string, std::mutex> key_mutexes;
class redisCmd{
public:
    redisCmd();
    int assignId();
    void setNewUser(nlohmann::json &data);
    int handleLogin(nlohmann::json &data);
    void returnUser(nlohmann::json &data);
    int Vuser(nlohmann::json &data);
    bool isAccount(std::string account);
    void reviseData(std::string key, std::string type,std::string typedata);
    void deleteUser(std::string account);
    void addFriend(std::string account, std::string friendname);
    void updataship(nlohmann::json &data);
    void waitHandleMeg(std::string key,nlohmann::json &data);
    cpp_redis::reply findmess(std::string name);
    std::string getData(std::string account, std::string dataname);
    int verifyUser(nlohmann::json &data);
    int see(std::string data);
    bool isfriend(std::string account,std::string name);
    void storeMessages(std::string sender, std::string account,std::string message);
    void sendOfflineMeg(nlohmann::json &data,const TcpConnectionPtr conn);
    void storeReadMeg(std::vector<std::string> message, std::string key);
    void sendHistoryMeg(nlohmann::json &data, const TcpConnectionPtr conn);
    void sendHisOffineMeg(nlohmann::json &data, const TcpConnectionPtr &conn);
    void delFriend(std::string user, std::string name);
    void black(std::string user, std::string name);
    void sendBlackMeg(nlohmann::json &data, const TcpConnectionPtr conn);
    void createGroup(nlohmann::json &data);
    void joinGroup(std::string key, std::string account,std::string rank);
    bool isgroup(std::string name);
    void delPerson(std::string name, std::string account, std::string rank);
    cpp_redis::reply seeGroup(std::string key);
    bool ismygroup(std::string name, std::string account);
    void storeGroupMeg(nlohmann::json &data, std::string msg);
    void getGroupMeg(std::string key, std::string name, const TcpConnectionPtr conn,int amount);
    void hset(std::string key, std::string field, std::string count);
    void hdel(std::string key, std::string field);
    std::string getLevel(std::string groupname, std::string username);
    void delmember(std::string group, std::string user);
    void hpush(std::string key, std::string meg);
    bool lookmess(std::string key, nlohmann::json &data);
    int getVerifyLen(std::string key);
    void sendHisMeg1(nlohmann::json &data, const TcpConnectionPtr conn);

private:
    void
    connect();
    int getRedisCount(std::string key, std::string field);
    cpp_redis::reply getRedisResult(std::string key, std::string field);

    std::string getField(const std::string &account, const std::string &field);
    cpp_redis::client redisClient;
};
#endif