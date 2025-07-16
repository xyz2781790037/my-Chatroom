#ifndef USERUI_H
#define USERUI_H
#include "../base/user.h"
#include <string>
const std::string COLOUR1 = "\033[1;34m";//蓝
const std::string COLOUREND = "\033[0m";
const std::string COLOUR2 = "\033[1;31m";//32深绿
const std::string COLOUR3 = "\033[1;38m";// 37白色
class Userui{
public:
    Userui(std::shared_ptr<User> user, const mulib::net::TcpClient::TcpConnectionPtr &Conn);

    void ui();
    void online(std::string ship);

private:
    void selectFunc(std::string select);
    void seefriend();
    void addfriend();
    void myinformation();
    void deleteUser();
    void viewInformation();
    bool handleCmd(std::string cmd);

    std::string concealPwd();
    bool Presence = true;
    std::shared_ptr<User> user_;
    const mulib::net::TcpClient::TcpConnectionPtr conn;
};

#endif