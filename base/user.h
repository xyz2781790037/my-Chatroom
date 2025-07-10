#ifndef USR_H
#define USR_H
#include <string>
#include <nlohmann/json.hpp>
#include <../netlib/net/TcpClient.h>
class User{
public:
    User(std::string usrname, std::string usrpassword, std::string usrqqemail);
    User(std::string usrname, std::string usrpassword);
    void sendUserInformation(const mulib::net::TcpClient::TcpConnectionPtr &conn);
    void sendLogin(const mulib::net::TcpClient::TcpConnectionPtr &conn);

private:
    std::string usrName;
    std::string usrPassword;
    std::string usrqqEmail;
};
inline User::User(std::string usrname, std::string usrpassword, std::string usrqqemail) : usrName(usrname),
usrPassword(usrpassword),usrqqEmail(usrqqemail){}
User::User(std::string usrname, std::string usrpassword) : usrName(usrname),usrPassword(usrpassword){}
inline void User::sendUserInformation(const mulib::net::TcpClient::TcpConnectionPtr &conn)
{
    srand(time(0));
    int randomNum = rand() % 99999999 + 1;
    std::string strNum = std::to_string(randomNum);
    int len = strNum.size();
    std::string myname = std::string(8 - len, '0') + strNum;
    nlohmann::json user;
    user["type"] = "register";
    user["account"] = usrName;
    user["password"] = usrPassword;
    user["qqEmail"] = usrqqEmail;
    user["myname"] = myname;
    conn->send(user.dump());
}
void User::sendLogin(const mulib::net::TcpClient::TcpConnectionPtr &conn){
    nlohmann::json user;
    user["type"] = "login";
    user["account"] = usrName;
    user["password"] = usrPassword;
    conn->send(user.dump());
}
#endif