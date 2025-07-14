#include "MegType.h"
#include "../netlib/base/logger.h"
namespace Type{
    std::atomic<Status> currentState_(Status::WAIT);
    std::atomic<UserStatus> currentUserState_(UserStatus::UWAIT);
    Status getState(){
        return currentState_.load(std::memory_order_acquire);
    }
    void updataState(Status state){
        currentState_.store(state, std::memory_order_release);
    }
    void updataUserState(UserStatus state){
        currentUserState_.store(state, std::memory_order_release);
    }
    UserStatus getUserState(){
        return currentUserState_.load(std::memory_order_release);
    }
}

Type::types Type::getDataType(std::string datatype)
{
    if (datatype == "register"){
        return REGISTER;
    }
    else if (datatype == "login"){
        return LOGIN;
    }
    else if (datatype == "getpwd"){
        return GETPWD;
    }
    else if (datatype == "print"){
        return PRINT;
    }
    else if (datatype == "information"){
        return INFOEMATION;
    }
    else if(datatype == "revise"){
        return REVISE;
    }
    else{
        LOG_FATAL << "getType error";
    }
}
