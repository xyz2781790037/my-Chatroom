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
    else if(datatype == "delete"){
        return DELETE;
    }
    else if(datatype == "add"){
        return ADD;
    }
    else if(datatype == "ship"){
        return SHIP;
    }
    else if(datatype == "messdata"){
        return MESSDATA;
    }
    else if(datatype == "verify"){
        return VERIFY;
    }
    else if(datatype == "see"){
        return SEE;
    }
    else if(datatype == "chat"){
        return CHAT;
    }
    else if(datatype == "message"){
        return MESSAGE;
    }
    else if(datatype == "offmeg"){
        return OFFMEG;
    }
    else{
        LOG_FATAL << "getType error";
    }
}
