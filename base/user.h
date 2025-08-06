#ifndef USR_H
#define USR_H
#include <string>
#include <nlohmann/json.hpp>
#include <../netlib/net/TcpClient.h>
#include "../netlib/base/logger.h"
#include "MessageSplitter.h"
class User{
public:
    User(std::string usrname, std::string usrpassword, std::string usrqqemail);
    User(std::string usrname, std::string usrpassword);
    void sendUserInformation(const mulib::net::TcpClient::TcpConnectionPtr &conn);
    void sendLogin(const mulib::net::TcpClient::TcpConnectionPtr &conn);
    static void getQQemail(std::string &account, mulib::net::TcpClient::TcpConnectionPtr &conn);
    static void sendPassword(std::string &account, std::string vcode, const mulib::net::TcpClient::TcpConnectionPtr &conn);
    static void resend(std::string &account, const mulib::net::TcpClient::TcpConnectionPtr &conn);
    void updataUserInformation(std::string usrname,std::string usrid);
    void preparation(nlohmann::json &j, std::string type, std::string typedata);
    void send(nlohmann::json &j, const mulib::net::TcpClient::TcpConnectionPtr &conn, std::string category);
    void send(nlohmann::json &j, const mulib::net::TcpClient::TcpConnectionPtr &conn);
    static void send(std::string qq, const mulib::net::TcpClient::TcpConnectionPtr &conn);

    std::string getUserName();
    std::string getUserEmail();
    std::string getUserMyname();
    std::string getUserId();
    std::string getPassword();

    void revisePwd(std::string password);
    void reviseMyname(std::string myname);

private:
    std::string usrName;
    std::string usrPassword;
    std::string usrqqEmail;
    std::string usrMyname;
    std::string usrId;
};
inline User::User(std::string usrname, std::string usrpassword, std::string usrqqemail) : usrPassword(usrpassword),usrqqEmail(usrqqemail){
    if(usrname.substr(0,5) == "user:"){
        usrName = usrname.substr(5);
    }
    else{
        usrName = usrname;
    }
}
inline User::User(std::string usrname, std::string usrpassword) : usrName(usrname),usrPassword(usrpassword){
    if (usrname.substr(0, 5) == "user:"){
        usrName = usrname.substr(5);
    }
    else{
        usrName = usrname;
    }
}
inline void User::sendUserInformation(const mulib::net::TcpClient::TcpConnectionPtr &conn)
{
    srand(time(0));
    int randomNum = rand() % 99999999 + 1;
    std::string strNum = std::to_string(randomNum);
    int len = strNum.size();
    std::string myname = std::string(8 - len, '0') + strNum;
    nlohmann::json user;
    user["type"] = "register";
    user["account"] = "user:" + usrName;
    user["password"] = usrPassword;
    user["qqEmail"] = usrqqEmail;
    user["myname"] = myname;
    conn->send(MessageSplitter::encodeMessage(user.dump()));
}
inline void User::sendLogin(const mulib::net::TcpClient::TcpConnectionPtr &conn){
    nlohmann::json user;
    user["type"] = "login";
    user["account"] = "user:" + usrName;
    user["password"] = usrPassword;
    conn->send(MessageSplitter::encodeMessage(user.dump()));
}
inline void User::sendPassword(std::string &account, std::string vcode,const mulib::net::TcpClient::TcpConnectionPtr &conn)
{
    nlohmann::json user;
    user["type"] = "getpwd";
    user["account"] = "user:" + account;
    user["return"] = "test";
    user["vcode"] = vcode;
    conn->send(MessageSplitter::encodeMessage(user.dump()));
}
inline void User::getQQemail(std::string &account, mulib::net::TcpClient::TcpConnectionPtr &conn){
    nlohmann::json user;
    user["type"] = "getpwd";
    user["account"] = "user:" + account;
    user["return"] = "email";
    conn->send(MessageSplitter::encodeMessage(user.dump()));
}
inline void User::resend(std::string &account, const mulib::net::TcpClient::TcpConnectionPtr &conn){
    nlohmann::json user;
    user["type"] = "getpwd";
    user["account"] = "user:" + account;
    user["return"] = "verify";
    conn->send(MessageSplitter::encodeMessage(user.dump()));
}
inline void User::updataUserInformation(std::string usrname, std::string usrid){
    usrMyname = usrname;
    usrId = usrid;
}
inline std::string User::getUserName(){
    return usrName;
}
inline std::string User::getUserEmail(){
    return usrqqEmail;
}
inline std::string User::getUserMyname(){
    return usrMyname;
}
inline std::string User::getUserId(){
    return usrId;
}
inline std::string User::getPassword(){
    return usrPassword;
}
inline void User::preparation(nlohmann::json &j, std::string type, std::string typedata){
    j[type] = typedata;
}
inline void User::send(nlohmann::json &j,const mulib::net::TcpClient::TcpConnectionPtr &conn,std::string category)
{
    j["account"] = category + usrName;
    LOG_DEBUG << j.dump();
    if(conn && conn->connected()){
        conn->send(MessageSplitter::encodeMessage(j.dump()));
        LOG_DEBUG << "111";
    }
    else{
        LOG_ERROR << "conn过期";
    }
}
inline void User::send(nlohmann::json &j, const mulib::net::TcpClient::TcpConnectionPtr &conn){
    LOG_DEBUG << j.dump();
    if(conn && conn->connected()){
        conn->send(MessageSplitter::encodeMessage(j.dump()));
        LOG_DEBUG << "111";
    }
    else{
        LOG_ERROR << "conn过期";
    }
}
inline void User::revisePwd(std::string password){
    usrPassword = password;
}
inline void User::reviseMyname(std::string myname){
    usrMyname = myname;
}
inline void User::send(std::string qq, const mulib::net::TcpClient::TcpConnectionPtr &conn){
    nlohmann::json j;
    j["account"] = qq;
    j["type"] = "getpwd";
    j["return"] = "register";
    conn->send(MessageSplitter::encodeMessage(j.dump()));
}
#endif