#include "UserUi.h"
#include "../base/logOn.h"
Userui::Userui(User &user, const mulib::net::TcpClient::TcpConnectionPtr &Conn) : user(user),conn(Conn){}
void Userui::ui()
{
    Presence = true;
    while(Presence){
        if (Type::getUserState() == Type::UEXECUTE)
        {
            std::cout << COLOUR1 << "     你好, " << user.getUserName() << COLOUREND << std::endl;
            std::cout << "     1.我的好友" << std::endl;
            std::cout << "     2.添加好友" << std::endl;
            std::cout << "     3.管理好友" << std::endl;
            std::cout << "     4.我的群聊" << std::endl;
            std::cout << "     5.添加群聊" << std::endl;
            std::cout << "     6.管理群聊" << std::endl;
            std::cout << "     7.验证信息" << std::endl;
            std::cout << "     8.我的信息" << std::endl;
            std::cout << "     9.退出登陆" << std::endl;
            std::string select;
            std::cin >> select;
            selectFunc(select);
        }
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
    }
    else if (select == "8"){
        myinformation();
    }
    else if (select == "9"){
        Presence = false;
        Type::updataState(Type::EXECUTE);
    }
    else{
        std::cout << COLOUR2 << "输入错误，请重新输入" << COLOUREND << std::endl;
        sleep(1);
        system("clear");
    }
}
std::string Userui::concealPwd(){
    std::string a;
    for (int i = 0; i < user.getPassword().size();i++){
        a += "*";
    }
    return a;
}
void Userui::myinformation(){
    std::cout << COLOUR3 << "账号：" << user.getUserName() << COLOUREND << std::endl;
    std::cout << COLOUR3 << "ID ：" << user.getUserId() << COLOUREND << std::endl;
    std::cout << COLOUR3 << "邮箱：" << user.getUserEmail() << COLOUREND << std::endl;
    std::cout << COLOUR3 << "用户名：" << user.getUserMyname() << COLOUREND << std::endl;
    std::cout << COLOUR3 << "密码：" << concealPwd() << COLOUREND << std::endl;
    std::cout << COLOUR3 << "输入1修改密码，输入2修改用户名，输入3退出" << COLOUREND << std::endl;
    std::string select1;
    std::cin >> select1;
    MessageSplitter::ignoreCin();
    if(select1 == "1"){
        std::string oldPassword,newPassword;
        while(1){
            std::cout << "请输入原密码：";
            getline(std::cin,oldPassword);
            if(oldPassword == user.getPassword()){
                std::cout << "请输入新密码：";
                getline(std::cin, newPassword);
                nlohmann::json j;
                j["type"] = "";
                j["password"] = newPassword;
                j["account"] = "user:" + user.getUserName();
                conn->send(j.dump() + "\n");
                break;
            }
            else{
                std::cout << "密码错误，请重新输入" << std::endl;
            }
        }
    }
    else if (select1 == "2"){
        std::cout << "请输入新的用户名：";
        std::string newmyname;
        getline(std::cin, newmyname);
    }
    else if (select1 == "3"){
        Type::updataUserState(Type::UEXECUTE);
    }
    else{
        myinformation();
        std::cout << "输入错误！" << std::endl;
    }
}