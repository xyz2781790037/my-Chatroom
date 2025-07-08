#ifndef LOG_ON_H
#define LOG_ON_H

#include <iostream>
#include "../netlib/base/logger.h"
#include <unistd.h>
#include <string>
#include <memory>
#include "user.h"
class logon{
public:
    void ui();
private:
    void selectFunc(int funcnum);
    void login();
    void Register();
    void exitSystem();
    void sendPerson(int fd, std::string personinformation);
};
inline void logon::ui(){
    std::cout << "   chatroom" << std::endl;
    std::cout << "   1.登陆" << std::endl;
    std::cout << "   2.注册" << std::endl;
    std::cout << "   3.退出" << std::endl;
    int funcNum;
    std::cin >> funcNum;
    selectFunc(funcNum);
}
inline void logon::selectFunc(int funcnum){
    switch (funcnum)
    {
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
        LOG_WARN << "输入错误,请重新输入";
        sleep(1);
        system("clear");
        ui();
        break;
    }
}
inline void logon::login(){
    std::string Account, passWord;
    std::cout << "账号: ";
    getline(std::cin, Account);
    std::cout << "密码: ";
    getline(std::cin, passWord);
    //
}
inline void logon::Register(){
    
    std::string registerAccount, registerPassword1, registerPassword2;
    std::string qqEmail;
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
    std::string personInformation = person.userInformation();
    sendPerson(1, personInformation);
    std::cout << "注册成功！" << std::endl;
}
inline void logon::exitSystem(){
    exit(0);
}
inline void sendPerson(int fd,std::string personinformation){

}
#endif