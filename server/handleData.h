#ifndef HANDLEDATA_H
#define HANDLEDATA_H
#include <string>
#include "redisCmd.h"
class handleData{
public:
    int getDataType(std::string datatype);
private:
    enum type
    {
        REGISTER,
        LOGIN,
        RETRIEVE_PWD,
    };
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
}
#endif