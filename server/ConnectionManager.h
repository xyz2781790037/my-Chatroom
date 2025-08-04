#ifndef CONNECTIONMANAGER_H
#define CONNECTIONMANAGER_H
#include "../netlib/net/TcpServer.h"
#include <unordered_map>

class ConnectionManager{
public:
    void addUserConn(const std::string &username, const mulib::net::TcpConnectionPtr &conn);
    void removeUserConn(const std::string &username,const mulib::net::TcpConnectionPtr &conn);
    mulib::net::TcpConnectionPtr getConn(const std::string &username) const;
    bool isOnline(const std::string &username) const;
    std::string getName(const mulib::net::TcpConnectionPtr &conn);

private:
    std::unordered_map<std::string, mulib::net::TcpConnectionPtr> userConnMap_;
    std::unordered_map<mulib::net::TcpConnectionPtr, std::string> connUserMap_;
    mutable std::mutex mutex_;
};

#endif