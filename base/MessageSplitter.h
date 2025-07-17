#ifndef MESSAGR_SPLITTER_H
#define MESSAGR_SPLITTER_H

#include "../netlib/net/Buffer.h"
#include <string>
#include <queue>

class MessageSplitter
{
public:
    // 添加数据源 buffer
    void append(mulib::net::Buffer *buf);

    bool nextMessage(std::string &msg);
    static std::string segstrspace(std::string &order, int count = 0);
    static void ignoreCin();
    static void safeGetline(std::string &line);

private:
    std::string buffer_;
    std::queue<std::string> messages_;
};
inline void MessageSplitter::append(mulib::net::Buffer *buf)
{
    buffer_ += buf->retrieveAllAsString();
    size_t pos;
    while ((pos = buffer_.find('\n')) != std::string::npos)
    {
        std::string msg = buffer_.substr(0, pos);
        buffer_.erase(0, pos + 1);
        if (!msg.empty())
        {
            messages_.push(msg);
        }
    }
}
inline bool MessageSplitter::nextMessage(std::string &msg)
{
    if (!messages_.empty())
    {
        msg = messages_.front();
        messages_.pop();
        return true;
    }
    return false;
}
inline std::string MessageSplitter::segstrspace(std::string &order, int count)
{
    int len = order.size();
    while (count < len)
    {
        if ((order[count] == ' '))
        {
            order.erase(count, 1);
        }
        else
        {
            count++;
        }
    }
    return order;
}
inline void MessageSplitter::ignoreCin(){
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}
inline void MessageSplitter::safeGetline(std::string &line)
{
    while (true) {
        if (!std::getline(std::cin, line)) {
            if (std::cin.eof()) {
                std::cin.clear();
                std::cout << "\n[提示] Ctrl+D 输入被忽略，请重新输入：";
            } else {
                std::cin.clear();
                std::cout << "\n[错误] 输入失败，请重新输入：";
            }
        } else {
            break; // 成功读入
        }
    }
}

#endif