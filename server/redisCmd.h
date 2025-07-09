#ifndef REDISCMD_H
#define REDISCMD_H
#include <cpp_redis/cpp_redis>
class redisCmd{
public:
    redisCmd();
    void assignId();
private:
    cpp_redis::client redisClient;
};
#endif