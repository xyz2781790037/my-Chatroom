#include "UserUi.h"
#include "../base/logOn.h"
#include "sendFile.h"
std::atomic<bool> uiStart;
MessageManager megManager_;
MessageManager groupMegManager_;

std::atomic<bool> messageReminder(false);
std::atomic<bool> gmessageReminder(false);
std::condition_variable chatCv;
std::condition_variable gchatCv;

std::unordered_map<std::string,bool> chatName;
std::mutex mtx;

Userui::Userui(std::shared_ptr<User> user, const mulib::net::TcpClient::TcpConnectionPtr &Conn, const mulib::net::TcpClient::TcpConnectionPtr ftpConn, mulib::base::Timestamp recviveTime) : user_(user), conn(Conn), recviveTime_(recviveTime),
ftpConn(ftpConn) {}
void Userui::ui()
{
    Presence = true;
    online("online");
    uiStart = false;
    while (Presence){
        LOG_DEBUG << "type=" << Type::getUserState();
        if (Type::getUserState() == Type::UEXECUTE)
        {
            updataMessage();
            std::cout << COLOUR1 << "     你好, " << user_->getUserName() << COLOUREND << std::endl;
            std::cout << "     1.我的好友" << std::endl;
            std::cout << "     2.添加好友" << std::endl;
            std::cout << "     3.管理好友" << std::endl;
            std::cout << "     4.我的群聊" << std::endl;
            std::cout << "     5.加入群聊" << std::endl;
            std::cout << "     6.管理群聊" << std::endl;
            std::cout << "     7.验证信息" << std::endl;
            std::cout << "     8.我的信息" << std::endl;
            std::cout << "     9.退出登陆" << std::endl;
            std::cout << "     10.注销账号" << std::endl;
            char *input = readline("");
            if (!input)
            {
                std::cout << "读取失败，请重新输入。" << std::endl;
                continue;
            }
            std::string select(input);
            free(input);
            selectFunc(select);
        }
    }
}
void Userui::selectFunc(std::string select){
    MessageSplitter::segstrspace(select);
    if(select == "1"){
        seeFriend();
    }
    else if (select == "2"){
        addFriend();
    }
    else if (select == "3"){
        managerFriend();
    }
    else if (select == "4"){
        myGroup();
    }
    else if (select == "5"){
        joinGroup();
    }
    else if (select == "6"){
        managerGroup();
    }
    else if (select == "7"){
        viewInformation();
    }
    else if (select == "8"){
        myinformation();
    }
    else if (select == "\u001b" || select == "9")
    {

        online("offline");
        Presence = false;
        uiStart = true;
        Type::updataState(Type::EXECUTE);
        g_ui_cv.notify_one();
    }
    else if(select == "10"){
        deleteUser();
    }
    else{
        std::cout << COLOUR2 << "输入错误，请重新输入" << COLOUREND << std::endl;
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
    LOG_DEBUG << user_->getUserEmail();
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
            while (!tool::isValidInput(oldPassword, "^[a-zA-Z0-9_]{1,16}$")){
                std::cout << "输入不合法,请重新数入" << std::endl;
                getline(std::cin, oldPassword);
            }
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
        while (!tool::isValidInput(newmyname, "^[\u4e00-\u9fa5a-zA-Z0-9.]{1,16}$"))
        {
            std::cout << "输入不合法,请重新数入" << std::endl;
            std::cout << "请输入新的用户名：";
            getline(std::cin, newmyname);
        }
        nlohmann::json j;
        user_->preparation(j, "type", "revise");
        user_->preparation(j, "myname", newmyname);
        user_->send(j, conn,"user:");
        user_->reviseMyname(newmyname);
    }
    else if (select1 == "3" || select1 == "\u001b"){
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
    if(dd == "Y" || dd == "y" || dd == "\n"){
        nlohmann::json j;
        user_->preparation(j, "type", "delete");
        user_->send(j, conn, "user:");
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
void Userui::see(){
    nlohmann::json j;
    user_->preparation(j, "type", "see");
    user_->preparation(j, "see", "friend");
    user_->send(j, conn, "frie:");
}
void Userui::seeFriend(){
    Type::updataUserState(Type::USAT);
    std::string cmd;
    while (1)
    {
        if(Type::getUserState() == Type::USAT) {
            see();
            Type::updataUserState(Type::UWAIT);
        }
        else if (Type::getUserState() == Type::URETURN){
            
            std::cout << "请输入好友:";
            getline(std::cin, cmd);
            MessageSplitter::segstrspace(cmd);
            if (cmd == "/quit" || cmd == "\u001b"){
                Type::updataUserState(Type::UEXECUTE);
                break;
            }
            else{
                {
                    std::unique_lock<std::mutex> lock(mtx);
                    std::string key = tool::swapsort("user:" + user_->getUserName(), "user:" + cmd, "read:");
                    chatName[key] = true;
                }
                nlohmann::json j;
                user_->preparation(j, "type", "chat");
                user_->preparation(j, "name", "user:" + cmd);
                user_->send(j, conn, "frie:");
                Type::updataUserState(Type::UWAIT);
            }
        }
        else if(Type::getUserState() == Type::UCHAT){

            std::atomic<bool> chatting(true);
            char *a = new char[4096 * 4096];
            bool chatlong = false;
            std::mutex chatMtx;
            std::thread recvThread([&]()
                                   {
                while (chatting) {
                    std::queue<std::string> Messages = megManager_.fetchMessages(tool::swapsort("user:" + cmd,"user:" + user_->getUserName(),"read:"));
                    while(!Messages.empty()){
                        LOG_DEBUG << "\033[1;35m" << std::this_thread::get_id() << "\033[0m";
                        std::cout << "\33[2K\r";
                        std::cout << Messages.front() << std::endl;
                        Messages.pop(); 
                    }
                    {
                        std::unique_lock lock(chatMtx);
                        chatCv.wait(lock, [&chatting]
                                     { return messageReminder || chatting == false; });
                    }
                    if (chatting == false) break;
                    messageReminder = false;
                } });
            while (chatting){
                std::string message;
                // std::cout << "发送:";
                if(!chatlong){
                    getline(std::cin, message);
                }
                else{
                    a = readline("发送:");
                    message.append(a);
                }
                recviveTime_ = recviveTime_.now();
                if(message == "/quit"|| message == "\u001b"){
                    Type::updataUserState(Type::USAT);
                    chatting = false;
                    nlohmann::json j;
                    user_->preparation(j, "type", "endchat");
                    user_->preparation(j, "name", "user:" + cmd);
                    user_->preparation(j, "return", "1");
                    user_->send(j, conn, "user:");
                    cmd.clear();
                    chatCv.notify_one();
                    chatName[tool::swapsort("user:" + user_->getUserName(), "user:" + cmd, "read:")] = false;
                    break;
                }
                else if(message.empty()){
                    std::cout << "\033[A";   // 光标上移一行
                    std::cout << "\33[2K\r"; // 清除整行 + 回到
                    continue;
                }
                else if(message[0] == '/'){
                    tool::segExcessiveSpace(message);
                    if(message == "/file"){
                        tool::clearInputLines(message);
                        fileSystem(" user:" + cmd);
                        tool::clearInputLines(message);
                    }
                    else if(message == "/long"){
                        chatlong = true;
                        continue;
                    }
                    else if(message == "/short"){
                        chatlong = false;
                        continue;
                    }
                    else if(message == "/h"){
                        nlohmann::json j;
                        user_->preparation(j, "type", "history");
                        std::string key = tool::swapsort("user:" + user_->getUserName(), "user:" + cmd, "read:");
                        user_->preparation(j, "account", key);
                        user_->send(j, conn);
                        std::cout << "正在加载历史消息" << std::endl;
                    }
                    else{
                        tool::clearInputLines(message);
                        std::cout << "输入错误" << std::endl;
                    }
                }
                else{
                    
                    nlohmann::json j;
                    user_->preparation(j, "type", "message");
                    user_->preparation(j, "to", "user:" + cmd);
                    std::string things;
                    if (cmd == user_->getUserName()){
                        things = headerFormat("You") + message;
                    }
                    else{
                        things = headerFormat(user_->getUserMyname()) + message;
                    }
                    user_->preparation(j, "things", things);
                    user_->preparation(j, "from", "user:" + user_->getUserName());
                    user_->send(j, conn);
                    tool::clear();
                    if (cmd != user_->getUserName()){
                        std::cout << headerFormat("You") << message << std::endl;
                    }
                }
            }
            free(a);
            recvThread.join();
        }
    }
}
void Userui::addFriend(){
    while(true){
        if(Type::getUserState() == Type::UEXECUTE){
            std::string name;
            std::cout << "请输入账号：";
            getline(std::cin, name);
            MessageSplitter::segstrspace(name);
            if (name == "/quit" || name == "\u001b"){
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
    Type::updataUserState(Type::UWAIT);
    while(1){
        if(Type::getUserState() == Type::UWAIT){
            nlohmann::json j;
            user_->preparation(j, "type", "messdata");
            user_->send(j, conn, "mess:user:");
            user_->send(j, conn, "mess:grop:");
            Type::updataUserState(Type::USAT);
        }
        if(Type::getUserState() == Type::URETURN){
            std::string cmd,name,result;
            std::cout << "输入1处理好友验证,输入2处理群聊验证,输入3处理邀请: ";
            getline(std::cin,cmd);
            if(cmd == "/quit" || cmd == "\u001b"){
                Type::updataUserState(Type::UEXECUTE);
                break;
            }
            else if(cmd == "1"){
                char* Name = readline("请选择好友(帐号):");
                name.append(Name);
                std::cout << "是否同意[Y/n]";
                getline(std::cin, result);
                if(tool::tolowerStr(result) == "y" || result.empty()){
                    
                    cmd = "/af " + name + " yes";
                }
                else if(tool::tolowerStr(result) == "n"){
                    cmd = "/af " + name + " no";
                }
                else{
                    cmd = "/af nn nn";
                }
                free(Name);
            }
            else if(cmd == "2"){
                char* Name = readline("请选择用户:");
                name.append(Name);
                Name = readline("请选择处理群聊:");
                name.append(":");
                name.append(Name);
                std::cout << "是否同意[Y/n]";
                getline(std::cin, result);
                if(tool::tolowerStr(result) == "y" || result.empty()){
                    
                    cmd = "/ag " + name + " yes";
                }
                else if(tool::tolowerStr(result) == "n"){
                    cmd = "/ag " + name + " no";
                }
                else{
                    cmd = "/af nn nn";
                }
                free(Name);
            }
            else if (cmd == "3")
            {
                char *Name = readline("请输入被邀请的群聊:");
                name.append(Name);
                std::cout << "是否同意[Y/n]";
                getline(std::cin, result);
                if (tool::tolowerStr(result) == "y" || result.empty())
                {

                    cmd = "/iv " + name + " yes";
                }
                else if(tool::tolowerStr(result) == "n")
                {
                    cmd = "/iv " + name + " no";
                }
                else{
                    cmd = "/af nn nn";
                }
                free(Name);
            }
            if(handleCmd(cmd)){
                Type::updataUserState(Type::USAT);
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
            if(pass == "/af" && (cmd.substr(end + 1) == "yes" || (cmd.substr(end + 1) == "no"))){
                user_->preparation(j, "account", user_->getUserName());
                user_->preparation(j, "pass", "addfriend");
                user_->preparation(j, "name", cmd.substr(start + 1, end - start - 1));
                user_->preparation(j, "result", cmd.substr(end + 1));
                user_->send(j, conn,"frie:");
                return true;
            }
            else if (pass == "/ag" && (cmd.substr(end + 1) == "yes" || (cmd.substr(end + 1) == "no"))){
                std::string owning = cmd.substr(start + 1, end - start - 1);
                user_->preparation(j, "name", owning.substr(0,owning.find_first_of(':')));
                user_->preparation(j, "pass", "addgroup");
                user_->preparation(j, "group", owning.substr(owning.find_first_of(':') + 1));
                user_->preparation(j, "result", cmd.substr(end + 1));
                user_->send(j, conn, "user:");
                return true;
            }
            else if(pass == "/iv" && (cmd.substr(end + 1) == "yes" || (cmd.substr(end + 1) == "no"))){
                user_->preparation(j, "name", cmd.substr(start + 1, end - start - 1));
                user_->preparation(j, "pass", "invitation");
                user_->preparation(j, "result", cmd.substr(end + 1));
                user_->send(j, conn, "mygp:");
                return true;
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
std::string Userui::headerFormat(std::string name){
    std::string time = recviveTime_.toFormattedString().substr(0, 16);
    return "[" + time + "-" + name + "]:";
}
void Userui::updataMessage(){
    nlohmann::json j;
    user_->preparation(j, "type", "offmeg");
    user_->preparation(j, "group", "mygp:" + user_->getUserName());
    user_->send(j, conn, "frie:");
}
void Userui::managerFriend(){
    while(true){
        std::cout << "1.删除好友" << std::endl;
        std::cout << "2.拉黑好友" << std::endl;
        std::cout << "3.黑名单" << std::endl;
        std::cout << "4.退出" << std::endl;
        std::string a, name;
        getline(std::cin, a);
        if(a == "1"){
            see();
            Type::updataUserState(Type::UWAIT);
            while(1){
                if(Type::getUserState() == Type::URETURN){
                    std::cout << "请选择你要删除的好友：";
                    getline(std::cin, name);
                    if (name == "\u001b"){
                        break;
                    }
                    else if(name == user_->getUserName()){
                        std::cout << "不能删除自己" << std::endl;
                        break;
                    }
                    nlohmann::json j;
                    user_->preparation(j, "type", "delfriend");
                    user_->preparation(j, "name", "user:" + name);
                    user_->send(j, conn, "frie:");
                    Type::updataUserState(Type::UEXECUTE);
                }
            }
        }
        else if(a == "2"){
            see();
            Type::updataUserState(Type::UWAIT);
            while(1){
                if (Type::getUserState() == Type::URETURN){
                    std::cout << "请选择你要拉黑的好友：";
                    getline(std::cin, name);
                    if (name == "\u001b"){
                        break;
                    }
                    else if (name == user_->getUserName())
                    {
                        std::cout << "不能拉黑自己" << std::endl;
                        break;
                    }
                    nlohmann::json j;
                    user_->preparation(j, "type", "black");
                    user_->preparation(j, "name", "user:" + name);
                    user_->send(j, conn, "frie:");
                    Type::updataUserState(Type::UEXECUTE);
                }
            }
            
        }
        else if(a == "3"){
            std::string blackname;
            nlohmann::json j;
            user_->preparation(j,"type","see");
            user_->preparation(j, "see", "black");
            user_->send(j, conn, "blak:");
            while(1){
                if(Type::getUserState() == Type::USAT){
                    nlohmann::json j;
                    user_->preparation(j, "type", "see");
                    user_->preparation(j, "see", "black");
                    user_->send(j, conn, "blak:");
                    Type::updataUserState(Type::UWAIT);
                }
                else if(Type::getUserState() == Type::URETURN){
                    std::cout << "请选择解除拉黑的好友:";
                    getline(std::cin, blackname);
                    if(blackname == "\u001b"){
                        break;
                    }
                    nlohmann::json j;
                    user_->preparation(j, "type", "black");
                    user_->preparation(j,"return","1");
                    user_->preparation(j, "name", "user:" + blackname);
                    user_->send(j, conn, "blak:");
                    Type::updataUserState(Type::UWAIT);
                }
            }
        }
        else if(a == "4" || a == "/quit" || a == "\u001b"){
            Type::updataUserState(Type::UEXECUTE);
            break;
        }
        else{
            std::cout << "输入错误" << std::endl;
        }
    }
}
void Userui::managerGroup(){
    while(true){
        Type::updataUserState(Type::UWAIT);
        std::cout << COLOUR4 << "1.创建群聊" << COLOUREND << std::endl;
        std::cout << COLOUR4 << "2.退出群聊" << COLOUREND << std::endl;
        std::cout << COLOUR4 << "3.退出" << COLOUREND << std::endl;
        std::string a, name,number;
        int count = 0;
        getline(std::cin, a);
        if(a == "\u001b" || a == "3" || a == "/quit"){
            Type::updataUserState(Type::UEXECUTE);
            break;
        }
        else if(a == "1"){
            std::cout << "请输入创建的群聊名称:";
            getline(std::cin, name);
            if(name == "\u001b"){
                continue;
            }
            nlohmann::json j;
            user_->preparation(j, "name", name);
            while(true){
                std::cout << "请选择加入的好友：";
                getline(std::cin, number);
                if(number == "\u001b"){
                    user_->preparation(j, "type", "creategroup");
                    user_->preparation(j, "count", std::to_string(count));
                    user_->send(j, conn, "user:");
                    break;
                }
                else if(number == user_->getUserName()){
                    continue;
                }
                user_->preparation(j, "member" + std::to_string(count), "user:" + number);
                count++;
            }
        }
        else if(a == "2"){
            std::cout << "请输入退出的群聊：";
            std::string group;
            getline(std::cin, group);
            nlohmann::json j;
            user_->preparation(j, "type", "delgroup");
            user_->preparation(j, "name", group);
            user_->send(j, conn, "user:");
            while(1){
                if(Type::getUserState() == Type::URETURN){
                    std::cout << COLOUR2 << "[WAIN] " << COLOUREND << "真的要继续吗[Y/n]";
                    std::string a;
                    getline(std::cin, a);
                    if(a == "Y" || a == "y" || a == "\n"){
                        user_->preparation(j, "return", "1");
                        user_->send(j, conn, "user:");
                        break;
                    }else{
                        break;
                    }
                }
            }
        }
        else{
            std::cout << COLOUR2 << "输入错误请重新输入" << COLOUREND << std::endl;
        }
    }
}
void Userui::myGroup(){
    while(true){
        if(Type::getUserState() == Type::UEXECUTE){
            std::cout << "1.群聊列表" << std::endl;
            std::cout << "2.群聊分类" << std::endl;
            std::cout << "3.退出" << std::endl;
            std::string a, name, number;
            getline(std::cin, a);
            nlohmann::json j;
            user_->preparation(j, "type", "seegroup");
            if (a == "\u001b" || a == "3" || a == "/quit"){
                Type::updataUserState(Type::UEXECUTE);
                break;
            }
            else if (a == "1"){
                user_->preparation(j, "rank", "all");
                user_->send(j, conn, "mygp:");
                std::mutex chatMtx;
                while(1){
                    if (Type::getUserState() == Type::URETURN){
                        std::cout << "请选择群聊：";
                        getline(std::cin, name);
                        MessageSplitter::segstrspace(name);
                        if (name == "/quit" || name == "\u001b"){
                            Type::updataUserState(Type::UEXECUTE);
                            break;
                        }
                        else{
                            nlohmann::json j;
                            user_->preparation(j, "type", "gchat");
                            user_->preparation(j, "name", "grop:" + name);
                            user_->send(j, conn, "mygp:");
                            Type::updataUserState(Type::UWAIT);
                        }
                    }
                    else if (Type::getUserState() == Type::UCHAT){
                        {
                            std::unique_lock<std::mutex> lock(mtx);
                            chatName["regp:" + name] = true;
                        }
                        std::atomic<bool> chatting(true);
                        std::thread recvThread([&](){
                            while (chatting) {
                                std::queue<std::string> Messages = groupMegManager_.fetchMessages("grop:" + name);
                                while(!Messages.empty()){
                                    std::cout << "\33[2K\r";
                                    std::cout << Messages.front() << std::endl;
                                    std::cout << "发送:";
                                    std::cout << "\033[0C" << std::flush;
                                    Messages.pop();
                                }
                                {
                                    std::unique_lock lock(chatMtx);
                                    gchatCv.wait(lock, [&chatting]
                                                 { return gmessageReminder || chatting == false; });
                                }
                                if (chatting == false) break;
                                gmessageReminder = false;
                            }
                        });
                        Type::updataUserState(Type::USAT);
                        while (chatting){
                            std::string message;
                            if (Type::getUserState() == Type::USAT){
                                std::cout << "发送:";
                                getline(std::cin, message);
                                recviveTime_ = recviveTime_.now();
                                if (message == "/quit" || message == "\u001b"){
                                    Type::updataUserState(Type::URETURN);
                                    chatting = false;
                                    nlohmann::json j;
                                    user_->preparation(j, "type", "endchat");
                                    user_->preparation(j, "group", "grop:" + name);
                                    user_->preparation(j, "return", "0");
                                    user_->send(j, conn, "user:");
                                    gchatCv.notify_one();
                                    chatName["regp:" + name] = false;
                                    break;
                                }else if(message.empty()){
                                std::cout << "\033[A";   // 光标上移一行
                                std::cout << "\33[2K\r"; // 清除整行 + 回到
                                }
                                else if (message[0] == '/'){
                                    Type::updataUserState(Type::UWAIT);
                                    message = tool::segExcessiveSpace(message);
                                    tool::clearInputLines(message);
                                    if(message == "/?"){
                                        std::cout << "/list" << std::endl;
                                        std::cout << "/file" << std::endl;
                                        std::cout << "/uplevel username" << std::endl;
                                        std::cout << "/downlevel username" << std::endl;
                                        std::cout << "/del username" << std::endl;
                                        Type::updataUserState(Type::USAT);
                                    }
                                    else if (message.size() < 5 || message.find_first_of(' ') != message.find_last_of(' ')){
                                        std::cout << "系统：" << COLOUR4 << "指令不合法!" << COLOUREND << std::endl;
                                        Type::updataUserState(Type::USAT);
                                    }
                                    else if(message == "/list"){
                                        nlohmann::json j;
                                        user_->preparation(j, "type", "list");
                                        std::string key = "grop:" + name;
                                        user_->preparation(j, "key", key);
                                        user_->send(j, conn, "2");
                                        while(1){
                                            if(Type::getUserState() == Type::URETURN){
                                                Type::updataUserState(Type::USAT);
                                                break;
                                            }
                                        }
                                    }
                                    else if(message == "/file"){
                                        fileSystem(" grop:" + name);
                                        Type::updataUserState(Type::USAT);
                                    }
                                    else if(message == "/h"){
                                        nlohmann::json j;
                                        user_->preparation(j, "type", "history");
                                        std::string key = "regp:" + name;
                                        user_->preparation(j, "account", key);
                                        user_->preparation(j, "name", "mygp:" + user_->getUserName());
                                        user_->send(j, conn);
                                        std::cout << "正在加载历史消息" << std::endl;
                                    }
                                    else if (message.substr(0, 5) == "/del "){
                                        nlohmann::json j;
                                        user_->preparation(j, "type", "cmd");
                                        user_->preparation(j, "name", "grop:" + name);
                                        user_->preparation(j, "use", "del");
                                        user_->preparation(j, "target", "user:" + message.substr(5));
                                        user_->send(j, conn, "user:");
                                    }
                                    else if (message.substr(0, 9) == "/uplevel "){
                                        nlohmann::json j;
                                        user_->preparation(j, "type", "cmd");
                                        user_->preparation(j, "name", "grop:" + name);
                                        user_->preparation(j, "use", "upl");
                                        user_->preparation(j, "target", "user:" + message.substr(9));
                                        user_->send(j, conn, "user:");
                                    }
                                    else if (message.substr(0, 11) == "/downlevel "){
                                        nlohmann::json j;
                                        user_->preparation(j, "type", "cmd");
                                        user_->preparation(j, "name", "grop:" + name);
                                        user_->preparation(j, "use", "dnl");
                                        user_->preparation(j, "target", "user:" + message.substr(11));
                                        user_->send(j, conn, "user:");
                                    }
                                    else{
                                        std::cout << "系统：" << COLOUR4 << "指令错误!" << COLOUREND << std::endl;
                                        Type::updataUserState(Type::USAT);
                                    }
                                }
                                else{
                                    nlohmann::json j;
                                    user_->preparation(j, "type", "gmessage");
                                    user_->preparation(j, "from", "grop:" + name);
                                    std::string things = headerFormat(user_->getUserMyname()) + message;
                                    user_->preparation(j, "things", things);
                                    user_->send(j, conn, "user:");
                                    tool::clear();
                                    std::cout << things << std::endl;
                                    
                                }
                            }
                                
                        }
                        recvThread.join();
                    }
                }
            }
            else if (a == "2"){
                Type::updataUserState(Type::URETURN);
                while (1){
                    if (Type::getUserState() == Type::URETURN){
                        Type::updataUserState(Type::UWAIT);
                        std::cout << "1.我创建的群聊" << std::endl;
                        std::cout << "2.我加入的群聊" << std::endl;
                        std::cout << "3.退出" << std::endl;
                        std::string b;
                        getline(std::cin, b);
                        if (b == "\u001b" || b == "3" || b == "/quit"){
                            Type::updataUserState(Type::UEXECUTE);
                            break;
                        }
                        else if (b == "1"){
                            user_->preparation(j, "rank", "owner");
                            user_->send(j, conn, "mygp:");
                            getchar();
                        }
                        else if (b == "2"){
                            user_->preparation(j, "rank", "member");
                            user_->send(j, conn, "mygp:");
                            getchar();
                        }
                        else{
                            Type::updataUserState(Type::URETURN);
                            std::cout << COLOUR2 << "输入错误" << COLOUREND << std::endl;
                        }
                    }
                }
            }
            else{
                std::cout << COLOUR2 << "输入错误" << COLOUREND << std::endl;
            }
        }
        
    }
    
}
void Userui::joinGroup(){
    while(1){
        if(Type::getUserState() == Type::UEXECUTE){
            std::cout << "1.请求加群" << std::endl;
            std::cout << "2.邀请进群" << std::endl;
            std::cout << "3.退出" << std::endl;
            std::string a, name, gname;
            getline(std::cin, a);
            if (a == "3" || a == "\u001b" || a == "/quit"){
                break;
            }
            else if (a == "1"){
                std::cout << "请输入群名:";
                getline(std::cin, name);
                nlohmann::json j;
                user_->preparation(j, "type", "add");
                user_->preparation(j, "name", name);
                user_->send(j, conn, "mygp:");
                Type::updataUserState(Type::UWAIT);
            }
            else if(a == "2"){
                std::cout << "请输入好友：";
                getline(std::cin, name);
                std::cout << "请选择群聊：";
                getline(std::cin, gname);
                nlohmann::json j;
                user_->preparation(j, "type", "add");
                user_->preparation(j, "name", gname);
                user_->preparation(j, "person", name);
                user_->send(j, conn, "grop:");
                Type::updataUserState(Type::UWAIT);
            }
            else{
                std::cout << COLOUR2 << "输入错误" << COLOUREND << std::endl;
            }
        }
        
    }
}
void Userui::fileSystem(std::string name){
    int fd = 0;
    while (true)
    {
        std::cout << COLOUR1 << "1.上传文件" << COLOUREND << std::endl;
        std::cout << COLOUR1 << "2.下载文件" << COLOUREND << std::endl;
        std::cout << COLOUR1 << "3.查看文件列表" << COLOUREND << std::endl;
        std::cout << COLOUR1 << "> " << COLOUREND;
        std::string input, fileName;
        getline(std::cin, input);
        if (input == "\u001b" || input == "/quit"){
            break;
        }
        else if(input == "3"){
            input = "list";
        }
        else if(input == "1" || input == "2"){
            char* name = readline("请输入文件路径或文件名:");
            fileName.append(name);
            if (input == "1")
            {
                input = "stor " + fileName;
            }
            else{
                input = "retr " + fileName;
            }
            free(name);
        }
        else{
            std::cout << "输入错误" << std::endl;
            continue;
        }
        LOG_DEBUG << input;
        if(input == tool::tolowerStr("LIST")){
            Type::updataUserState(Type::UWAIT);
            nlohmann::json j;
            user_->preparation(j, "type", "list");
            if(name.substr(1,5) == "user:"){
                std::string a = tool::swapsort("user:" + user_->getUserName(), name.substr(1), "file:");
                user_->preparation(j, "key", a);
            }
            else if(name.substr(1,5) == "grop:"){
                std::string a = "file:" + name.substr(6);
                user_->preparation(j, "key", a);
            }
            user_->send(j,conn,"1");
            while(1){
                if(Type::getUserState() == Type::URETURN){
                    break;
                }
            }
        }
        else{
            if(input.substr(0,4) == tool::tolowerStr("STOR")){
                std::string filename = input.substr(5);
                if ((fd = tool::getFilefd(filename)) < 0)
                {
                    tool::clearInputLines(input);
                    std::cout << "文件不存在" << std::endl;
                    continue;
                }
                if (ftpConn && ftpConn->connected()) {
                    ftpConn->send(MessageSplitter::encodeMessage(input));
                    ftpConn->send(MessageSplitter::encodeMessage("info " + user_->getUserName() + name + " " + user_->getUserMyname() + " " + tool::fileSize(fd) + " " + input.substr(5)));
                    tool::clearInputLines(input);
                } else {
                    LOG_ERROR << "conn过期";
                }
            }
            else if(input.substr(0,4) == tool::tolowerStr("RETR")){
                Type::updataUserState(Type::UWAIT);
                nlohmann::json j;
                user_->preparation(j, "type", "look");
                user_->preparation(j, "name", input.substr(5));
                if(name.substr(1,5) == "user:"){
                    std::string a = tool::swapsort("user:" + user_->getUserName(), name.substr(1), "file:");
                    user_->preparation(j, "key", a);
                }
                else if(name.substr(1,5) == "grop:"){
                    std::string a = "file:" + name.substr(6);
                    user_->preparation(j, "key", a);
                }
                user_->send(j, conn, "123");
                while (1){
                    if(Type::getUserState() == Type::URETURN){
                        if (ftpConn && ftpConn->connected()) {
                            ftpConn->send(MessageSplitter::encodeMessage(input));
                            tool::clearInputLines(input);
                        } else {
                            LOG_ERROR << "conn过期";
                        }
                        break;
                    }
                    else if(Type::getUserState() == Type::USAT){
                        std::cout << "文件不存在，无法下载" << std::endl;
                        break;
                    }
                }
                
            }
        }
    }
}