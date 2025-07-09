#ifndef USR_H
#define USR_H
#include <string>
#include <nlohmann/json.hpp>
#include <../netlib/net/TcpClient.h>
class User{
public:
    User(std::string usrname, std::string usrpassword, std::string usrqqemail);
    void sendUserInformation(const mulib::net::TcpClient::TcpConnectionPtr &conn);

private:
    std::string usrName;
    std::string usrPassword;
    std::string usrqqEmail;
};
User::User(std::string usrname, std::string usrpassword, std::string usrqqemail) : usrName(usrname),
usrPassword(usrpassword),usrqqEmail(usrqqemail){}
void User::sendUserInformation(const mulib::net::TcpClient::TcpConnectionPtr &conn)
{
    nlohmann::json user;
    user["type"] = "register";
    user["account"] = usrName;
    user["password"] = usrPassword;
    user["qqEmail"] = usrqqEmail;
    conn->send(user.dump());
}
#endif