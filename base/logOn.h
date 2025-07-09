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
    void ui(mulib::net::TcpClient &client);

private:
    void selectFunc(int funcnum, mulib::net::TcpClient &client);
    void login();
    void Register(mulib::net::TcpClient &client);
    void exitSystem();
    void sendPerson(int fd, std::string personinformation);
};
inline void logon::ui(mulib::net::TcpClient &client)
{
    while(1){
        std::cout << "   chatroom" << std::endl;
        std::cout << "   1.登陆" << std::endl;
        std::cout << "   2.注册" << std::endl;
        std::cout << "   3.退出" << std::endl;
        int funcNum;
        std::cin >> funcNum;
        selectFunc(funcNum,client);
    }
}
inline void logon::selectFunc(int funcnum, mulib::net::TcpClient &client)
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
        std::cout << "输入错误,请重新输入";
        sleep(1);
        system("clear");
        break;
    }
}
inline void logon::login(){
    std::string Account, passWord;
    getchar();
    std::cout << "账号: ";
    getline(std::cin, Account);
    std::cout << "密码: ";
    getline(std::cin, passWord);
    //
}
inline void logon::Register(mulib::net::TcpClient &client)
{

    std::string registerAccount, registerPassword1, registerPassword2;
    std::string qqEmail;
    getchar();
    std::cout << "账号: ";
    getline(std::cin, registerAccount);
    std::cout << "密码: ";
    getline(std::cin, registerPassword1);
    std::cout << "再输入一次密码: ";
    getline(std::cin, registerPassword2);
    while(registerPassword2 != registerPassword1){
        std::cout << "与密码不符，请重新输入";
        std::cout << "再输入一次密码: ";
        getline(std::cin, registerPassword2);
    }
    std::cout << "请绑定qq邮箱: ";
    getline(std::cin, qqEmail);
    User person = User(registerAccount, registerPassword1, qqEmail);
    if (client.connect() && client.connect())
    {
        User user;
        user.sendUserInformation(client.connection()); //  获取连接，传入你的函数
    }
    const mulib::net::TcpClient::TcpConnectionPtr conn;
    person.sendUserInformation(conn);
    std::cout << "注册成功！" << std::endl;
}
inline void logon::exitSystem(){
    exit(0);
}
inline void logon::sendPerson(int fd,std::string personinformation){
    int i = 1;
}
#endif