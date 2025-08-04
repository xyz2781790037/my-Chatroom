#ifndef SENDFILE_H
#define SENDFILE_H
#include "../netlib/base/logger.h"
#include "../netlib/net/TcpConnection.h"
#include "../netlib/net/TcpClient.h"
#include "../base/MegType.h"
#include "../base/MessageSplitter.h"
#include "../base/tool.h"
#include <unistd.h>
#include <fcntl.h>
#include <sys/sendfile.h>
#include <sys/stat.h>
#include <nlohmann/json.hpp>
#include <filesystem>
class sendFile
{
public:
    void sendMeg(std::string message);
    void recvMeg(MessageSplitter &megSpl, mulib::base::Timestamp recviveTime);
    void getConn(const mulib::net::TcpClient::TcpConnectionPtr conn);
    void getmainConn(const mulib::net::TcpClient::TcpConnectionPtr mainconn);
    mulib::net::TcpClient::TcpConnectionPtr returnConn();

private:
    void stor(int &fileFd, int &dataFd);
    void retr(int &fileFd, int &dataFd);
    mulib::net::TcpClient::TcpConnectionPtr ftpConn;
    mulib::net::TcpClient::TcpConnectionPtr mainConn;
    int createFile(std::string file);
    void sendmain(std::string fileName, std::string from, std::string to, std::string myname, std::string fileSize, mulib::base::Timestamp recviveTime);
};
#endif