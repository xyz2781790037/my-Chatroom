#ifndef LOG_ON_H
#define LOG_ON_H

#include <iostream>
#include <../netlib/net/TcpClient.h>
#include "../netlib/base/logger.h"
#include <unistd.h>
#include <string>
#include <memory>
#include <atomic>
#include "user.h"
class logon{
public:
    logon(std::shared_ptr<mulib::net::TcpClient> client) : client_(client) {}
    void ui();
    enum Status{
        WAIT,
        EXECUTE,
        RETURN
    };
    void updataState(Status state);
    Status getState() const;

private:
    void selectFunc(int funcnum);
    void login();
    void Register();
    void exitSystem();
    void getPassword();
    std::shared_ptr<mulib::net::TcpClient> client_;
    std::atomic<Status> currentState_;
};
inline void logon::ui()
{
    updataState(EXECUTE);
    while(true){
        if(getState() == EXECUTE){
            std::cout << "   chatroom" << std::endl;
            std::cout << "   1.登陆" << std::endl;
            std::cout << "   2.注册" << std::endl;
            std::cout << "   3.退出" << std::endl;
            std::cout << "   4.找回密码 " << std::endl;
            int funcNum;
            std::cin >> funcNum;
            selectFunc(funcNum);
        }
    }
}
inline logon::Status logon::getState() const{
    return currentState_.load(std::memory_order_acquire);
}
inline void logon::selectFunc(int funcnum)
{
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
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
    case 4:
        getPassword();
        break;
    default:
        std::cout << "输入错误,请重新输入" << std::endl;
        sleep(1);
        break;
    }
}
inline void logon::login(){
    updataState(WAIT);
    std::string Account, passWord;
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
    updataState(WAIT);
    std::string registerAccount, registerPassword1, registerPassword2;
    std::string qqEmail;
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
inline void logon::updataState(Status state){
    currentState_.store(state, std::memory_order_release);
}
inline void logon::getPassword(){
    updataState(WAIT);
    std::string account, qqemail, Vcode;
    std::cout << "输入你的账号：";
    getline(std::cin,account);
    auto conn = client_->connection();
    if (conn && conn->connected()){
        LOG_INFO << "开始进入服务器查找账号";
        User::getQQemail(account, conn);
        while(true){
            if (getState() == EXECUTE)
            {
                std::cout << "请输入验证码(输入/resend重发): ";
                getline(std::cin, Vcode);
                if(Vcode == "/resend"){
                    User::resend(account, conn);
                    updataState(WAIT);
                    continue;
                }
                User::sendPassword(account, Vcode, conn);
                updataState(WAIT);
            }
            else if(getState() == RETURN){
                updataState(EXECUTE);
                return;
            }
        }
    }
    else{
        std::cout << "服务器未运行，无法发送找回信息。\n";
    }
}
#endif