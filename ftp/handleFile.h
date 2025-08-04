#ifndef HANDLE_FILE_H
#define HANDLE_FILE_H
#include "../netlib/base/logger.h"
#include "../netlib/net/TcpConnection.h"
#include "../netlib/net/TcpServer.h"
#include "../netlib/net/Socket.h"
#include "../netlib/net/SocketOps.h"
#include "../base/MegType.h"
#include "../base/MessageSplitter.h"
#include "../base/tool.h"
#include <unistd.h>
#include <fcntl.h>
#include <atomic>
class handleFile{
public:
    void getConn(const std::shared_ptr<mulib::net::TcpConnection> conn, mulib::base::Timestamp recviveTime);
    void sendMeg(std::string message);
    void handleInput(std::string input);

private:

    void stor(int &fd, int &connFd);
    void retr(int &fd, int &connFd);
    std::string analysis(std::string &type,std::string input);
    int createFile(std::string file);
    int getFilefd(std::string filename);
    std::string connectPort();
    std::shared_ptr<mulib::net::TcpConnection> ftpConn;
    std::unique_ptr<mulib::net::Socket> acceptSocket_;   // 用于 listen 的 socket
    std::unique_ptr<mulib::net::Channel> acceptChannel_; // 接收连接的 Channel
    EventLoop *loop_;
    std::atomic<bool> flag;
    mulib::base::Timestamp recviveTime_;
    std::unordered_map<std::string, mulib::base::Timestamp> fileTimeMap;
};
#endif