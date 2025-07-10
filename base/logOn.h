#ifndef LOG_ON_H
#define LOG_ON_H

#include <iostream>
#include <../netlib/net/TcpClient.h>
#include "../netlib/base/logger.h"
#include <unistd.h>
#include <string>
#include <memory>
#include "user.h"
class logon{
public:
    logon(std::shared_ptr<mulib::net::TcpClient> client) : client_(client) {}
    void ui();

private:
    void selectFunc(int funcnum);
    void login();
    void Register();
    void exitSystem();
    // void sendPerson(int fd, std::string personinformation);
    std::shared_ptr<mulib::net::TcpClient> client_;
};
inline void logon::ui()
{
    while(1){
        std::cout << "   chatroom" << std::endl;
        std::cout << "   1.登陆" << std::endl;
        std::cout << "   2.注册" << std::endl;
        std::cout << "   3.退出" << std::endl;
        int funcNum;
        std::cin >> funcNum;
        selectFunc(funcNum);
    }
}
inline void logon::selectFunc(int funcnum)
{
    switch (funcnum){
    case 1:
        login();
        break;
    case 2:
        Register();
        break;
        case 3:
        exitSystem();
        break;
        default:
        std::cout << "输入错误,请重新输入" << std::endl;
        sleep(1);
        system("clear");
        break;
    }
}
inline void logon::login(){
    std::string Account, passWord;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cout << "账号: ";
    getline(std::cin, Account);
    std::cout << "密码: ";
    getline(std::cin, passWord);
    User user(Account,passWord);
    auto conn = client_->connection();
    if (conn)
    {
        user.sendLogin(conn);
    }
    else
    {
        std::cout << "服务器未运行，无法发送登陆信息。\n";
    }
}
inline void logon::Register()
{

    std::string registerAccount, registerPassword1, registerPassword2;
    std::string qqEmail;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cout << "账号: ";
    getline(std::cin, registerAccount);
    std::cout << "密码: ";
    getline(std::cin, registerPassword1);
    std::cout << "再输入一次密码: ";
    getline(std::cin, registerPassword2);
    while(registerPassword2 != registerPassword1){
        std::cout << "与密码不符，请重新输入" << std::endl;
        std::cout << "再输入一次密码: ";
        getline(std::cin, registerPassword2);
    }
    std::cout << "请绑定qq邮箱: ";
    getline(std::cin, qqEmail);
    User person(registerAccount, registerPassword1, qqEmail);
    auto conn = client_->connection();
    if (conn)
    {
        person.sendUserInformation(conn);
    }
    else
    {
        std::cout << "服务器未运行，无法发送注册信息。\n";
    }
}
inline void logon::exitSystem(){
    client_->disconnect();
    client_->stop();
    exit(0);
}

#endif