#include "MessageManager.h"
#include <iostream>
void MessageManager::pushMessage(const std::string &user, const std::string &msg){
    std::lock_guard<std::mutex> lock(mutex_);
    userMessage_[user].push(msg);
}
std::queue<std::string> MessageManager::fetchMessages(const std::string &user){
    std::lock_guard<std::mutex> lock(mutex_);
    auto &q = userMessage_[user];
    std::queue<std::string> msgs;
    std::swap(msgs, q);
    return msgs;
}
bool MessageManager::hasMessages(const std::string &user) const{
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = userMessage_.find(user);
    return it != userMessage_.end() && !it->second.empty();
}
std::vector<std::string> MessageManager::getUsersWithMessages() const{
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<std::string> users;
    for (const auto &pair : userMessage_){
        if (!pair.second.empty()){
            users.push_back(pair.first);
        }
    }
    return users;
}