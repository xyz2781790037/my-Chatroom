#include "handleFile.h"
#include "../netlib/net/TcpServer.h"
#include <future>
#include <sys/stat.h>
#include <sys/sendfile.h>
void handleFile::getConn(const std::shared_ptr<mulib::net::TcpConnection> conn, mulib::base::Timestamp recviveTime)
{
    ftpConn = conn;
    recviveTime_ = recviveTime;
}
void handleFile::sendMeg(std::string message){
    ftpConn->send(MessageSplitter::encodeMessage(message));
}
std::string handleFile::analysis(std::string &type, std::string input){
    type = input.substr(0,4);
    if(input.size() > 5){
        return input.substr(5);
    }
    return "";
}
int handleFile::createFile(std::string file){
    int pos = file.find_last_of('/');
    std::string fileName = file.substr(pos + 1);
    recviveTime_ = recviveTime_.now();
    fileTimeMap[fileName] = recviveTime_;
    fileName += recviveTime_.toFormattedString().substr(0, 19);
    chdir("/home/zgyx/myServer/uploads");
    int fileFd = open(fileName.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if(fileFd < 0){
        LOG_ERROR << "create file failed:" << strerror(errno);
        sendMeg("550 Failed to open file for writing.");
        return -1;
    }
    return fileFd;
}
void handleFile::handleInput(std::string input){
    std::string type;
    std::string fileName = analysis(type, input);
    std::shared_ptr<mulib::net::TcpConnection> Conn;
    LOG_INFO << "[" << type << "]";
    if(type == "stor"){
        int fileFd = createFile(fileName);
        LOG_INFO << fileName;
        if (fileFd > 0){
            std::string port = connectPort();
            std::string message = "150 " + port + ":" + input.substr(5);
            LOG_INFO << message;
            sendMeg(message);
            InetAddress peerAddr;
            int connFd = acceptSocket_->accept1(&peerAddr);
            if(connFd <= 0){
                LOG_ERROR << "accept fail->" << "\033[1;34m" << strerror(errno) << "\033[0m";
            }
            stor(fileFd,connFd);
        }
        else{
            LOG_ERROR << "创建文件失败";
            sendMeg("Failed to open file for writing.");
        }
    }
    else if(type == "retr"){
        int fileFd = getFilefd(fileName);
        LOG_INFO << fileName;
        if(fileFd > 0){
            std::string port = connectPort();
            std::string message = "151 " + port + ":" + input.substr(5);
            LOG_INFO << message;
            sendMeg(message);
            InetAddress peerAddr;
            int connFd = acceptSocket_->accept1(&peerAddr);
            retr(fileFd, connFd);
        }
    }
    else if(type == "info"){
        std::string message = "999 " + input.substr(5) + "\n\r" + fileTimeMap[input.substr(input.find_last_of('/') + 1)].toFormattedString().substr(0,19);
        LOG_INFO << message;
        sendMeg(message);
    }
    else if(type == "cloe"){
        if (acceptSocket_) {
            close(acceptSocket_->fd());
            acceptSocket_.reset();  // 释放资源
            LOG_INFO << "数据端口已关闭";
        }
    }
}
void handleFile::stor(int &fileFd,int &connFd){
    int read_bytes = 0;
    char buffers[1024];
    LOG_INFO << "read start";
    while ((read_bytes = read(connFd, buffers, sizeof(buffers))) > 0){
        if (write(fileFd, buffers, read_bytes) != read_bytes){
            LOG_ERROR << "write fail:" << strerror(errno);
            close(fileFd);
            return;
        }
    }
    if (read_bytes == 0){
        LOG_INFO << "read end";
        close(fileFd);
    }
    else{
        LOG_ERROR << "read fail:" << strerror(errno);
        close(fileFd);
    }
}
std::string handleFile::connectPort(){
    int dataFd = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (dataFd < 0){
        LOG_FATAL << "SocketOps: Create socket failed!";
        exit(EXIT_FAILURE);
    }
    InetAddress localAddr_(0);
    acceptSocket_ = std::make_unique<mulib::net::Socket>(dataFd);
    acceptSocket_->bindAddress(localAddr_.getSockAddr());
    acceptSocket_->listen();
    socklen_t len = sizeof(sockaddr_in);
    ::getsockname(dataFd, reinterpret_cast<sockaddr *>(const_cast<sockaddr_in *>(&localAddr_.getSockAddr())), &len);
    std::string portStr = localAddr_.toIpPort();
    LOG_INFO << "Listening on " << portStr;
    return portStr;
}
int handleFile::getFilefd(std::string filename)
{
    std::string path = "/home/zgyx/myServer/uploads/";
    path += filename;
    int fileFd = open(path.c_str(), O_RDONLY, 0644);
    if (fileFd < 0)
    {
        LOG_ERROR << "open failed:" << strerror(errno);
        return -1;
    }
    return fileFd;
}
void handleFile::retr(int &fd, int &connFd){
    struct stat st;
    fstat(fd, &st);
    off_t offset = 0;
    ssize_t bytes_sent = 0;
    while (offset < st.st_size)
    {
        bytes_sent = sendfile(connFd, fd, &offset, 1024);
        if (bytes_sent < 0)
        {
            std::cerr << "Error send file" << std::endl;
            close(fd);
            return;
        }
        else if (bytes_sent == 0)
        {
            break;
        }
    }
    close(fd);
    shutdown(connFd, SHUT_WR);
    LOG_INFO << "服务端发送完毕，关闭连接";
}