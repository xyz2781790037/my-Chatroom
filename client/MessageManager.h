#ifndef MESSAGEMANAGER_H
#define MESSAGEMANAGER_H
#include <unordered_map>
#include <queue>
#include <string>
#include <mutex>
class MessageManager{
public:
    bool pushMessage(const std::string &user, const std::string &msg, int CACHE_THRESHOLD);
    std::queue<std::string> fetchMessages(const std::string &user);
    bool hasMessages(const std::string &user) const;

private:
    std::unordered_map<std::string, std::queue<std::string>> userMessage_;
    mutable std::mutex mutex_;
};

#endif