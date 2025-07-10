#ifndef REDISCMD_H
#define REDISCMD_H
#include <cpp_redis/cpp_redis>
#include <nlohmann/json.hpp>
class redisCmd{
public:
    redisCmd();
    int assignId();
    void setNewUser(nlohmann::json data);
    int handleLogin(nlohmann::json data);
    void returnUser(std::string account, std::string qqemail);

private: 
    void connect();
    bool isAccount(std::string account);
    std::string getQQEmail(std::string account);
    std::string getMyname(std::string account);
    std::string getPassward(std::string account);
    std::string getID(std::string account);
    std::string getField(const std::string &account, const std::string &field);
    cpp_redis::client redisClient;
};
#endif