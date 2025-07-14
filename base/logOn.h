#ifndef LOG_ON_H
#define LOG_ON_H

#include <iostream>
#include <../netlib/net/TcpClient.h>
#include "../netlib/base/logger.h"
#include <unistd.h>
#include <string>
#include <memory>
#include "user.h"
#include "MessageSplitter.h"
#include "MegType.h"
class logon{
public:
    logon(std::shared_ptr<mulib::net::TcpClient> client) : client_(client) {}
    void ui();

private:
    void selectFunc(std::string funcnum);
    void login();
    void Register();
    void exitSystem();
    void getPassword();
    std::shared_ptr<mulib::net::TcpClient> client_;
};
inline void logon::ui()
{
    Type::updataState(Type::Status::EXECUTE);
    while(true){
        if (Type::getState() == Type::EXECUTE)
        {
            std::cout << "   chatroom" << std::endl;
            std::cout << "   1.登陆" << std::endl;
            std::cout << "   2.注册" << std::endl;
            std::cout << "   3.退出" << std::endl;
            std::cout << "   4.找回密码 " << std::endl;
            std::string funcNum;
            std::cin >> funcNum;
            selectFunc(funcNum);
        }
    }
}
inline void logon::selectFunc(std::string funcnum)
{
    MessageSplitter::ignoreCin();
    if (funcnum == "1"){
        login();
    }
    else if(funcnum == "2"){
        Register();
    }
    else if(funcnum == "3"){
        exitSystem();
    }
    else if(funcnum == "4"){
        getPassword();
    }
    else{
        std::cout << "输入错误,请重新输入" << std::endl;
        sleep(1);
        system("clear");
    }
}
inline void logon::login(){
    Type::updataState(Type::Status::WAIT);
    std::string Account, passWord;
    std::cout << "账号: ";
    getline(std::cin, Account);
    MessageSplitter::segstrspace(Account);
    std::cout << "密码: ";
    getline(std::cin, passWord);
    MessageSplitter::segstrspace(passWord);
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
    Type::updataState(Type::Status::WAIT);
    std::string registerAccount, registerPassword1, registerPassword2;
    std::string qqEmail;
    std::cout << "账号: ";
    getline(std::cin, registerAccount);
    MessageSplitter::segstrspace(registerAccount);
    std::cout << "密码: ";
    getline(std::cin, registerPassword1);
    MessageSplitter::segstrspace(registerPassword1);
    std::cout << "再输入一次密码: ";
    getline(std::cin, registerPassword2);
    MessageSplitter::segstrspace(registerPassword2);
    while(registerPassword2 != registerPassword1){
        std::cout << "与密码不符，请重新输入" << std::endl;
        std::cout << "再输入一次密码: ";
        getline(std::cin, registerPassword2);
        MessageSplitter::segstrspace(registerPassword2);
    }
    std::cout << "请绑定qq邮箱: ";
    getline(std::cin, qqEmail);
    MessageSplitter::segstrspace(qqEmail);
    User person(registerAccount, registerPassword1, qqEmail);
    auto conn = client_->connection();
    if (conn){
        person.sendUserInformation(conn);
    }
    else{
        std::cout << "服务器未运行，无法发送注册信息。\n";
    }
}
inline void logon::exitSystem(){
    client_->disconnect();
    client_->stop();
    exit(0);
}
inline void logon::getPassword(){
    Type::updataState(Type::Status::WAIT);
    std::string account, qqemail, Vcode;
    std::cout << "输入你的账号：";
    getline(std::cin,account);
    auto conn = client_->connection();
    if (conn && conn->connected()){
        LOG_INFO << "开始进入服务器查找账号";
        User::getQQemail(account, conn);
        while(true){
            if (Type::getState() == Type::EXECUTE)
            {
                std::cout << "请输入验证码(输入/resend重发): ";
                getline(std::cin, Vcode);
                if(Vcode == "/resend"){
                    User::resend(account, conn);
                    Type::updataState(Type::Status::WAIT);
                    continue;
                }
                User::sendPassword(account, Vcode, conn);
                Type::updataState(Type::Status::WAIT);
            }
            else if (Type::getState() == Type::RETURN){
                Type::updataState(Type::Status::EXECUTE);
                return;
            }
        }
    }
    else{
        std::cout << "服务器未运行，无法发送找回信息。\n";
    }
}
#endif