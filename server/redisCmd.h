#ifndef REDISCMD_H
#define REDISCMD_H
#include <cpp_redis/cpp_redis>
#include <nlohmann/json.hpp>
class redisCmd{
public:
    redisCmd();
    int assignId();
    void setNewUser(nlohmann::json &data);
    int handleLogin(nlohmann::json &data);
    void returnUser(nlohmann::json &data);
    int Vuser(nlohmann::json &data);
    bool isAccount(std::string account);
    void reviseData(nlohmann::json &data, std::string type,std::string typedata);
    std::string getQQEmail(std::string account);
    std::string getPassward(std::string account);
    void deleteUser(std::string account);
    int addFriend(std::string account, std::string friendname);
    void updataship(nlohmann::json &data);

private: 
    void connect();
    std::string getUserStatus(std::string account);
    std::string getMyname(std::string account);
    std::string getID(std::string account);
    std::string getField(const std::string &account, const std::string &field);
    cpp_redis::client redisClient;
};
#endif