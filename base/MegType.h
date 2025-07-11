#ifndef MEGTYPE_H
#define MEGTYPE_H
#include <string>
namespace Type{
    enum types
    {
        REGISTER,
        LOGIN,
        GETPWD,
        PRINT,
        INFOEMATION
    };
    types getDataType(std::string datatype);
}
using namespace Type;
types Type::getDataType(std::string datatype)
{
    if (datatype == "register")
    {
        return REGISTER;
    }
    else if (datatype == "login")
    {
        return LOGIN;
    }
    else if (datatype == "getpwd")
    {
        return GETPWD;
    }
    else if (datatype == "print")
    {
        return PRINT;
    }
    else if (datatype == "information")
    {
        return INFOEMATION;
    }
}
#endif