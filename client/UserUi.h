#ifndef USERUI_H
#define USERUI_H
const std::string COLOUR1 = "\033[1;34m";
const std::string COLOUREND = "\033[0m";
#include "../base/user.h"
class Userui{
public:
    Userui(User &user);
    void ui(User &user);

private:
    void selectFunc(std::string select);
    void myinformation();
    bool Presence = true;
    User &user;
};

#endif