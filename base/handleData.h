#ifndef HANDLEDATA_H
#define HANDLEDATA_H
#include <string>
#include "redisCmd.h"
class handleData{
public:
    int getDataType(std::string datatype);
    enum type
    {
        REGISTER,
        LOGIN,
        RETRIEVE_PWD,
        PRINT
    };
private:
    void handleRegister();
};
inline int handleData::getDataType(std::string datatype){
    if(datatype == "register"){
        return REGISTER;
    }
    else if(datatype == "login"){
        return LOGIN;
    }
    else if(datatype == "retrieve_pwd"){
        return RETRIEVE_PWD;
    }
    else if(datatype == "print"){
        return PRINT;
    }
}
void handleData::handleRegister(){
    redis.setNewUser(jsonData);
    LOG_INFO << "准备发送注册成功消息给客户端";
    if (conn && conn->connected())
    {
        conn->send("注册成功！");
        LOG_INFO << "end";
    }
    else
    {
        std::cout << "连接尚未建立，无法发送注册信息。\n";
    }
}
#endif