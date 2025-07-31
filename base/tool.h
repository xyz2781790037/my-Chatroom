#ifndef TOOL_H
#define TOOL_H
#include <string>
#include <cctype>
#include <sys/ioctl.h>
#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <iomanip>
#include <regex>
#include "../netlib/base/logger.h"
namespace tool{
    std::string swapsort(std::string a, std::string b, std::string type);
    std::string spellName(std::string a,std::string b,std::string type);
    std::string segExcessiveSpace(std::string &order, int count = 0);
    bool compareLevel(std::string a, std::string b);
    std::string tolowerStr(std::string a);
    int getTerminalWidth();
    void clearInputLines(const std::string &input);
    int getFilefd(std::string filename);
    std::string fileSize(int fileFd);
    void clear();
    bool isValidInput(const std::string &input, std::string patterns);
}

#endif