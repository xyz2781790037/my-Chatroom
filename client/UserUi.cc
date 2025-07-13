#include "UserUi.h"
Userui::Userui(User &user) : user(user){}
void Userui::ui()
{
    Presence = true;
    while(Presence){
        std::cout << COLOUR1 << "你好, " << user.usrName << COLOUREND << std::endl;
        std::cout << "     1.我的好友" << std::endl;
        std::cout << "     2.添加好友" << std::endl;
        std::cout << "     3.管理好友" << std::endl;
        std::cout << "     3.我的群聊" << std::endl;
        std::cout << "     4.添加群聊" << std::endl;
        std::cout << "     5.管理群聊" << std::endl;
        std::cout << "     6.验证信息" << std::endl;
        std::cout << "     7.我的信息" << std::endl;
        std::cout << "     9.退出登陆" << std::endl;
        std::string select;
        std::cin >> select;
        selectFunc(select); 
    }
}
void Userui::selectFunc(std::string select){
    if(select == "1"){

    }
    else if (select == "2"){
    }
    else if (select == "3"){
    }
    else if (select == "4"){
    }
    else if (select == "5"){
    }
    else if (select == "6"){
    }
    else if (select == "7"){
        myinformation();
    }
    else if (select == "8"){
    }
    else if (select == "9"){
    }
    else{

    }
}
void Userui::myinformation(){
    std::cout << "账号：" << user.getUserName();
}