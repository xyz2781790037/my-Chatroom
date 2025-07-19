#include "MessageManager.h"
#include "TermiosGuard.h"
#include <iostream>
TermiosGuard guard;
bool MessageManager::pushMessage(const std::string &user, const std::string &msg, int CACHE_THRESHOLD)
{
    std::lock_guard<std::mutex> lock(mutex_);
    userMessage_[user].push(msg);
    auto it = userMessage_.find(user);
    if(it->second.size() >= CACHE_THRESHOLD){
        return true;
    }
    return false;
}
std::queue<std::string>& MessageManager::fetchMessages(const std::string &user){
    std::lock_guard<std::mutex> lock(mutex_);
    return userMessage_[user];
}
bool MessageManager::hasMessages(const std::string &user) const{
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = userMessage_.find(user);
    return it != userMessage_.end() && !it->second.empty();
}