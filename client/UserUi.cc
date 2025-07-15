#include "UserUi.h"
#include "../base/logOn.h"
Userui::Userui(std::shared_ptr<User> user, const mulib::net::TcpClient::TcpConnectionPtr &Conn) : user_(user), conn(Conn) {}
Userui::~Userui() {
    std::cout << "Userui 被析构，conn 生命周期结束" << std::endl;
}

void Userui::ui()
{
    Presence = true;
    online("online");
    while (Presence){
        if (Type::getUserState() == Type::UEXECUTE)
        {
            std::cout << COLOUR1 << "     你好, " << user_->getUserName() << COLOUREND << std::endl;
            std::cout << "     1.我的好友" << std::endl;
            std::cout << "     2.添加好友" << std::endl;
            std::cout << "     3.管理好友" << std::endl;
            std::cout << "     4.我的群聊" << std::endl;
            std::cout << "     5.添加群聊" << std::endl;
            std::cout << "     6.管理群聊" << std::endl;
            std::cout << "     7.验证信息" << std::endl;
            std::cout << "     8.我的信息" << std::endl;
            std::cout << "     9.退出登陆" << std::endl;
            std::cout << "     10.注销账号" << std::endl;
            std::string select;
            std::cin >> select;
            selectFunc(select);
        }
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
}
void Userui::selectFunc(std::string select){
    MessageSplitter::ignoreCin();
    if(select == "1"){

    }
    else if (select == "2"){
        std::string name;
        std::cout << "请输入账号：";
        getline(std::cin, name);
        nlohmann::json j;
        user_->preparation(j, "type", "add");
        user_->preparation(j, "name", name);
        user_->send(j, conn, "fire:");
        Type::updataUserState(Type::UWAIT);
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
        online("offline");
        Presence = false;
        Type::updataState(Type::EXECUTE);
    }
    else if(select == "10"){
        deleteUser();
    }
    else{
        std::cout << COLOUR2 << "输入错误，请重新输入" << COLOUREND << std::endl;
        sleep(1);
        system("clear");
    }
}
std::string Userui::concealPwd(){
    std::string a;
    for (int i = 0; i < user_->getPassword().size();i++){
        a += "*";
    }
    return a;
}
void Userui::myinformation(){
    LOG_INFO << user_->getUserEmail();
    std::cout << COLOUR3 << "账号：" << user_->getUserName() << COLOUREND << std::endl;
    std::cout << COLOUR3 << "ID ：" << user_->getUserId() << COLOUREND << std::endl;
    std::cout << COLOUR3 << "邮箱：" << user_->getUserEmail() << COLOUREND << std::endl;
    std::cout << COLOUR3 << "用户名：" << user_->getUserMyname() << COLOUREND << std::endl;
    std::cout << COLOUR3 << "密码：" << concealPwd() << COLOUREND << std::endl;
    std::cout << COLOUR3 << "输入1修改密码，输入2修改用户名，输入3退出" << COLOUREND << std::endl;
    std::string select1;
    std::cin >> select1;
    Type::updataUserState(Type::UWAIT);
    MessageSplitter::ignoreCin();
    if (select1 == "1"){
        std::string oldPassword,newPassword;
        while(1){
            std::cout << "请输入原密码：";
            getline(std::cin,oldPassword);
            if(oldPassword == user_->getPassword()){
                std::cout << "请输入新密码：";
                getline(std::cin, newPassword);
                nlohmann::json j;
                user_->preparation(j, "type", "revise");
                user_->preparation(j, "password", newPassword);
                user_->send(j, conn,"user:");
                user_->revisePwd(newPassword);
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
        nlohmann::json j;
        user_->preparation(j, "type", "revise");
        user_->preparation(j, "myname", newmyname);
        user_->send(j, conn,"user:");
        user_->reviseMyname(newmyname);
    }
    else if (select1 == "3"){
        Type::updataUserState(Type::UEXECUTE);
    }
    else{
        
        std::cout << "输入错误！" << std::endl;
        myinformation();
    }
}
void Userui::deleteUser(){
    std::cout << "请输入账号：";
    std::string account, pwd,dd;
    getline(std::cin, account);
    if(account != user_->getUserName()){
        std::cout << COLOUR2 << "输入错误，请稍后再试" << COLOUREND << std::endl;
        return;
    }
    std::cout << "请输入密码：";
    getline(std::cin, pwd);
    if(pwd != user_->getPassword()){
        std::cout << COLOUR2 << "输入错误，请稍后再试" << COLOUREND << std::endl;
        return;
    }
    std::cout << "真的要注销" << user_->getUserName() << "这个账号吗[Y/n]";
    getline(std::cin, dd);
    if(dd == "Y"){
        nlohmann::json j;
        user_->preparation(j, "type", "delete");
        user_->send(j, conn,"user:");
        Presence = false;
    }
    else{
        return;
    }
}
void Userui::online(std::string ship){
    nlohmann::json j;
    user_->preparation(j, "mystate", ship);
    user_->preparation(j, "type", "ship");
    user_->send(j, conn,"user:");
}