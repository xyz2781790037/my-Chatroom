#ifndef MEGTYPE_H
#define MEGTYPE_H
#include <string>
#include <atomic>
namespace Type{
    enum types
    {
        REGISTER,
        LOGIN,
        GETPWD,
        PRINT,
        INFOEMATION,
        REVISE
    };
    enum Status
    {
        WAIT,
        EXECUTE,
        RETURN
    };
    enum UserStatus
    {
        UWAIT,
        UEXECUTE,
        URETURN
    };
    void updataState(Status state);
    Status getState();
    void updataUserState(UserStatus state);
    UserStatus getUserState();
    types getDataType(std::string datatype);
    
    extern std::atomic<Status> currentState_;
    extern std::atomic<UserStatus> currentUserState_;
}
#endif