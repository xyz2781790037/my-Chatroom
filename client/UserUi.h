#ifndef USERUI_H
#define USERUI_H
#include "../base/user.h"
#include <string>
#include "MessageManager.h"
#include "../base/tool.h"
extern std::atomic<bool> uiStart;
extern MessageManager megManager_;
extern MessageManager groupMegManager_;
extern std::atomic<bool> messageReminder;
extern std::condition_variable chatCv;
const std::string COLOUR1 = "\033[1;34m";//蓝
const std::string COLOUREND = "\033[0m";
const std::string COLOUR2 = "\033[1;31m";//32深绿
const std::string COLOUR3 = "\033[1;38m";// 37白色
const std::string COLOUR4 = "\033[1;33m"; // 黄色
class Userui{
public:
    Userui(std::shared_ptr<User> user, const mulib::net::TcpClient::TcpConnectionPtr &Conn, const mulib::net::TcpClient::TcpConnectionPtr ftpConn, mulib::base::Timestamp recviveTime);

    void ui();

private:
    void selectFunc(std::string select);
    void seeFriend();
    void addFriend();
    void managerFriend();
    void myinformation();
    void deleteUser();
    void viewInformation();
    void see();
    bool handleCmd(std::string cmd);
    void managerGroup();
    void myGroup();
    void joinGroup();
    void fileSystem(std::string name);

    void online(std::string ship);
    std::string headerFormat(std::string name);
    void updataMessage();

    std::string concealPwd();
    bool Presence = true;
    std::shared_ptr<User> user_;
    const mulib::net::TcpClient::TcpConnectionPtr conn;
    mulib::base::Timestamp recviveTime_;
    mulib::net::TcpClient::TcpConnectionPtr ftpConn;
};

#endif