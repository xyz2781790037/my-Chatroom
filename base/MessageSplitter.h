#ifndef MESSAGR_SPLITTER_H
#define MESSAGR_SPLITTER_H
#include "TermiosGuard.h"
#include "../netlib/net/Buffer.h"
#include <string>
#include <queue>
#include <cstdint>
#include <cstring>
#include <limits>
#include "threadPool.h"
extern TermiosGuard guard;
class MessageSplitter
{
public:
    // 添加数据源 buffer
    void append(std::string buffer);

    bool nextMessage(std::string &msg);
    static std::string encodeMessage(const std::string &msg);
    static std::string segstrspace(std::string &order, int count = 0);
    static void ignoreCin();

private:
    std::string buffer_;
    std::queue<std::string> messages_;
    ThreadPool pool;
};
inline void MessageSplitter::append(std::string buffer)
{
    buffer_ += buffer;
    while(true){
        if (buffer_.size() < sizeof(int32_t))
            return;

        int32_t len;
        std::memcpy(&len, buffer_.data(), sizeof(int32_t));
        len = ntohl(len);

        if (len < 0 || static_cast<size_t>(len) > buffer_.size() - sizeof(int32_t))
        {
            // 等待更多数据
            return;
        }

        std::string msg = buffer_.substr(sizeof(int32_t), len);
        buffer_.erase(0, sizeof(int32_t) + len);
        // LOG_INFO << msg;
        // LOG_INFO << "\033[1;35m" << buffer_ << "\033[0m";
        messages_.push(msg);
    }
    
}
inline std::string MessageSplitter::encodeMessage(const std::string &msg)
{
    int32_t len = static_cast<int32_t>(msg.size());
    int32_t netlen = htonl(len);
    std::string out;
    out.append(reinterpret_cast<char *>(&netlen), sizeof(netlen));
    out.append(msg);
    return out;
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

#endif