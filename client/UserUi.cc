#include "UserUi.h"
#include "../base/logOn.h"
MessageManager megManager_;
Userui::Userui(std::shared_ptr<User> user, const mulib::net::TcpClient::TcpConnectionPtr &Conn) : user_(user), conn(Conn) {}

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
    MessageSplitter::segstrspace(select);
    if(select == "1"){
        seefriend();
    }
    else if (select == "2"){
        addfriend();
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
        viewInformation();
    }
    else if (select == "8"){
        myinformation();
    }
    else if (select == "\u0003" || select == "9")
    {

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
            guard.encryption();
            std::cout << "请输入原密码：";
            getline(std::cin,oldPassword);
            guard.removeEncryption();
            if(oldPassword == user_->getPassword()){
                std::cout << "\n请输入新密码：";
                getline(std::cin,newPassword);
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
        getline(std::cin,newmyname);
        nlohmann::json j;
        user_->preparation(j, "type", "revise");
        user_->preparation(j, "myname", newmyname);
        user_->send(j, conn,"user:");
        user_->reviseMyname(newmyname);
    }
    else if (select1 == "3" || select1 == "\u0003"){
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
    getline(std::cin,account);
    MessageSplitter::segstrspace(account);
    if(account != user_->getUserName()){
        std::cout << COLOUR2 << "输入错误，请稍后再试" << COLOUREND << std::endl;
        return;
    }
    std::cout << "请输入密码：";
    getline(std::cin,pwd);
    if(pwd != user_->getPassword()){
        std::cout << COLOUR2 << "输入错误，请稍后再试" << COLOUREND << std::endl;
        return;
    }
    std::cout << "真的要注销" << user_->getUserName() << "这个账号吗[Y/n]";
    getline(std::cin,dd);
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
void Userui::seefriend(){
    nlohmann::json j;
    user_->preparation(j, "account",user_->getUserName());
    user_->preparation(j, "type", "see");
    user_->preparation(j, "see", "friend");
    user_->send(j, conn, "frie:");
    Type::updataUserState(Type::UWAIT);
    std::string cmd;
    while (1)
    {
        if(Type::getUserState() == Type::URETURN){
            std::vector<std::string> users;
            users = megManager_.getUsersWithMessages();
            for(auto user : users){
                std::cout << "你收到了" << user.substr(5) << "的消息" << std::endl;
            }
        }
        if (Type::getUserState() == Type::URETURN){
            
            std::cout << "请输入好友:";
            getline(std::cin, cmd);
            MessageSplitter::segstrspace(cmd);
            if (cmd == "/quit" || cmd == "\u0003"){
                Type::updataUserState(Type::UEXECUTE);
                break;
            }
            else{
                nlohmann::json j;
                user_->preparation(j, "type", "chat");
                user_->preparation(j, "account",user_->getUserName());
                user_->preparation(j, "name", "user:" + cmd);
                user_->send(j, conn, "frie:");
                Type::updataUserState(Type::UWAIT);
            }
        }
        else if(Type::getUserState() == Type::UCHAT){
            std::atomic<bool> chatting(true);
            std::thread recvThread([&]()
                                   {
                while (chatting) {
                    std::queue<std::string> Messages;
                    Messages = megManager_.fetchMessages("user:" + cmd);
                    while(!Messages.empty()){
                        std::cout << "\33[2K\r";
                        std::cout << Messages.front() << std::endl;
                        std::cout << "发送";
                        std::cout << "\033[3C" << std::flush;
                        Messages.pop();
                    }
                    std::this_thread::sleep_for(std::chrono::milliseconds(200));
                } });
            while (chatting){
                std::string message;
                std::cout << "发送:";
                getline(std::cin, message);
                if(message == "/quit"|| message == "\u0003"){
                    Type::updataUserState(Type::URETURN);
                    chatting = false;
        
                    cmd.clear();
                    break;
                }
                else{
                    if(cmd != user_->getUserName()){
                        nlohmann::json j;
                        user_->preparation(j, "type", "message");
                        user_->preparation(j, "account", user_->getUserName());
                        user_->preparation(j, "receive", "user:" + cmd);
                        std::string things = "[" + user_->getUserMyname() + "]:" + message;
                        user_->preparation(j, "things", things);
                        user_->send(j, conn, "user:");
                    }
                    std::cout << "\033[A";   // 光标上移一行
                    std::cout << "\33[2K\r"; // 清除整行 + 回到行首
                    // 打印格式化后的聊天消息
                    std::cout << "[" << user_->getUserMyname() << "]:" << message << std::endl;
                }
            }
            recvThread.join();
        }
    }
}
void Userui::addfriend(){
    while(true){
        if(Type::getUserState() == Type::UEXECUTE){
            std::string name;
            std::cout << "请输入账号：";
            getline(std::cin, name);
            MessageSplitter::segstrspace(name);
            if (name == "/quit" || name == "\u0003"){
                break;
            }
            nlohmann::json j;
            user_->preparation(j, "type", "add");
            user_->preparation(j, "name", name);
            user_->send(j, conn, "frie:");
            Type::updataUserState(Type::UWAIT);
        }
    }
    
}
void Userui::viewInformation(){
    nlohmann::json j;
    user_->preparation(j, "type", "messdata");
    user_->send(j, conn, "mess:user:");
    Type::updataUserState(Type::UWAIT);
    while(1){
        if(Type::getUserState() == Type::URETURN){
            std::string cmd;
            std::cout << "请输入指令(/addfriend username yes/no): ";
            getline(std::cin,cmd);
            if(cmd == "/quit" || cmd == "\u0003"){
                Type::updataUserState(Type::UEXECUTE);
                break;
            }
            if(handleCmd(cmd)){
                Type::updataUserState(Type::UWAIT);
            }
        }
    }
}
bool Userui::handleCmd(std::string cmd){
    if(cmd[0] == '/'){
        int pos1 = cmd.find_first_of(' ');
        if(cmd.find(' ',pos1 + 1) == cmd.find_last_of(' ')){
            std::string pass = cmd.substr(0,cmd.find_first_of(' '));
            nlohmann::json j;
            int start = cmd.find_first_of(' ');
            int end = cmd.find_last_of(' ');
            user_->preparation(j,"type","verify");
            if(pass == "/addfriend" && (cmd.substr(end + 1) == "yes" || (cmd.substr(end + 1) == "no"))){
                user_->preparation(j, "account", user_->getUserName());
                user_->preparation(j, "name", cmd.substr(start + 1, end - start - 1));
                user_->preparation(j, "result", cmd.substr(end + 1));
                user_->send(j, conn,"");
                return true;
            }
            else if(pass == "/addgroup"){

            }
            else{
                std::cout << "输入错误" << std::endl;
                return false;
            }
        }
        else{
            std::cout << "格式错误" << std::endl;
            return false;
        }
    }
    else{
        std::cout << "输入错误,请重新输入" << std::endl;
        return false;
    }
}