#include "tool.h"
std::string tool::swapsort(std::string a, std::string b, std::string type){
    a = a.substr(5);
    b = b.substr(5);
    if (a > b)
    {
        std::swap(a, b);
    }
    return type + a + ":" + b;
}