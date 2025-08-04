#include "MessageManager.h"
#include "TermiosGuard.h"
#include <iostream>
TermiosGuard guard;
bool MessageManager::pushMessage(const std::string &user, const std::string &msg, int CACHE_THRESHOLD) {
    std::lock_guard<std::mutex> lock(mutex_);

    auto it = userMessage_.find(user);
    if (it != userMessage_.end()) {
        if (it->second.size() >= CACHE_THRESHOLD) {
            return true;
        }
        it->second.push(msg);
    } else {
        userMessage_[user].push(msg);
    }
    return false;
}
std::queue<std::string> MessageManager::fetchMessages(const std::string &user){
    std::lock_guard<std::mutex> lock(mutex_);
    std::queue<std::string> result;
    auto it = userMessage_.find(user);
    if (it != userMessage_.end())
    {
        std::swap(result, it->second); // 这一步清空了原队列
    }
    return result;
}
bool MessageManager::hasMessages(const std::string &user) const{
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = userMessage_.find(user);
    return it != userMessage_.end() && !it->second.empty();
}