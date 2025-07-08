#ifndef USR_H
#define USR_H
#include <string>
class User{
public:
    User(std::string usrname, std::string usrpassword, std::string usrqqemail);
    std::string userInformation();

private:
    std::string usrName;
    std::string usrPassword;
    std::string usrqqEmail;
};
User::User(std::string usrname, std::string usrpassword, std::string usrqqemail) : usrName(usrname),
usrPassword(usrpassword),usrqqEmail(usrqqemail){}
std::string User::userInformation(){
    return usrName + "\n" + usrPassword + "\n" + usrqqEmail;
}
#endif