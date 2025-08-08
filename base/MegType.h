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
        REVISE,
        DELETE,
        ADD,
        SHIP,
        MESSDATA,
        VERIFY,
        SEE,
        CHAT,
        MESSAGE,
        OFFMEG,
        ENDCHAT,
        DELFRIEND,
        BLACK,
        CREATEGROUP,
        DELGROUP,
        SEEGROUP,
        GCHAT,
        GMESSAGE,
        CMD,
        FILE,
        LOOK,
        LIST,
        TCP,
        HISTORY
    };
    enum Status
    {
        WAIT,
        EXECUTE,
        RETURNS
    };
    enum UserStatus
    {
        UWAIT,
        UEXECUTE,
        URETURN,
        UCHAT,
        USAT,
        UEND
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