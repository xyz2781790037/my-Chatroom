#ifndef MESSAGEMANAGER_H
#define MESSAGEMANAGER_H
#include <unordered_map>
#include <queue>
#include <string>
#include <mutex>
class MessageManager{
public:
    void pushMessage(const std::string &user, const std::string &msg);
    std::queue<std::string> fetchMessages(const std::string &user);
    bool hasMessages(const std::string &user) const;
    std::vector<std::string> getUsersWithMessages() const;

private:
    std::unordered_map<std::string, std::queue<std::string>> userMessage_;
    mutable std::mutex mutex_;
};

#endif