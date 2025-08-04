#include "ConnectionManager.h"
void ConnectionManager::addUserConn(const std::string &username, const mulib::net::TcpConnectionPtr &conn){
    std::lock_guard<std::mutex> lock(mutex_);
    userConnMap_[username] = conn;
    connUserMap_[conn] = username;
}
void ConnectionManager::removeUserConn(const std::string &username,const mulib::net::TcpConnectionPtr &conn){
    std::lock_guard<std::mutex> lock(mutex_);
    userConnMap_.erase(username);
    connUserMap_.erase(conn);
}
mulib::net::TcpConnectionPtr ConnectionManager::getConn(const std::string &username) const{
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = userConnMap_.find(username);
    if (it != userConnMap_.end())
    {
        return it->second;
    }
    return nullptr;
}
bool ConnectionManager::isOnline(const std::string &username) const{
    std::lock_guard<std::mutex> lock(mutex_);
    return userConnMap_.count(username) > 0;
}
std::string ConnectionManager::getName(const mulib::net::TcpConnectionPtr &conn){
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = connUserMap_.find(conn);
    if (it != connUserMap_.end())
    {
        return it->second;
    }
    return "null";
}