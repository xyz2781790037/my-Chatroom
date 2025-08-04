#include "sendFile.h"
#include <sys/stat.h>
#include "../base/tool.h"
void sendFile::sendMeg(std::string message){
    ftpConn->send(MessageSplitter::encodeMessage(message));
}
void sendFile::recvMeg(MessageSplitter &megSpl,mulib::base::Timestamp recviveTime){
    std::string buf;
    while (megSpl.nextMessage(buf)){
        LOG_DEBUG << buf;
        if (buf.substr(0, 3) == "150" || buf.substr(0, 3) == "151")
        {
            int pos = buf.find_first_of(':');
            int portPos = buf.find(':',pos + 1);
            std::string ip = buf.substr(4, pos - 4);
            std::string port = buf.substr(pos + 1,portPos - pos - 1);
            int fileFd = 0;
            if(buf.substr(0,3) == "150"){
                fileFd = tool::getFilefd(buf.substr(portPos + 1));
            }
            else{
                fileFd = createFile(buf.substr(portPos + 1));
            }
            if(fileFd < 0){
                sendMeg("cloe");
                LOG_DEBUG << "文件打开失败";
                continue;
            }
            int dataFd = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
            if (dataFd < 0){
                LOG_FATAL << "SocketOps: Create socket failed!";
                exit(EXIT_FAILURE);
            }
            sockaddr_in clientAddr;
            clientAddr.sin_family = AF_INET;
            clientAddr.sin_port = htons(std::stoi(port));
            inet_pton(AF_INET, "10.30.0.127", &clientAddr.sin_addr);
            int flags = fcntl(dataFd, F_GETFL, 0);
            fcntl(dataFd, F_SETFL, flags & ~O_NONBLOCK);
            if(::connect(dataFd, (sockaddr*)&clientAddr,sizeof(clientAddr)) < 0){
                LOG_ERROR << "connect fail:" << "\033[1;34m" << strerror(errno) << "\033[0m";
                continue;
            }
            LOG_DEBUG << "connect success";
            if (buf.substr(0, 3) == "150"){
                stor(fileFd, dataFd);
            }
            else{
                retr(fileFd, dataFd);
            }
        }
        else if(buf.substr(0,3) == "550"){
            std::cout << "\033[1;31m[ERROR]\033[0m " << buf << std::endl;
        }
        else if(buf.substr(0,3) == "999"){
            int pos0 = buf.find("\n\r");
            std::string filetime = buf.substr(pos0 + 2);
            buf = buf.substr(0, pos0);
            int pos1 = buf.find(' ', 4);
            std::string from = buf.substr(4, pos1 - 4);
            int pos2 = buf.find(' ', pos1 + 1);
            std::string to = buf.substr(pos1 + 1, pos2 - pos1 - 1);
            int pos3 = buf.find(' ', pos2 + 1);
            std::string myname = buf.substr(pos2 + 1,pos3 - pos2 - 1);
            int pos33 = buf.find(' ', pos3 + 1);
            int pos4 = buf.find(' ', pos33 + 1);
            std::string fileSize = buf.substr(pos3 + 1, pos4 - pos3 - 1);
            std::string fileName = buf.substr(pos4 + 1);
            int pos5 = fileName.find_last_of('/');
            
            fileName = fileName.substr(pos5 + 1);
            int pos6 = fileName.find_last_of('.');
            fileName = fileName.substr(0,pos6) + filetime + fileName.substr(pos6);
            sendmain(fileName, from, to, myname, fileSize, recviveTime);
        }
    }
}
void sendFile::stor(int &fileFd,int &dataFd){
    struct stat st;
    fstat(fileFd, &st);
    off_t offset = 0;
    ssize_t bytes_sent = 0;
    while (offset < st.st_size){
        bytes_sent = sendfile(dataFd, fileFd, &offset, 1024);
        if (bytes_sent < 0){
            std::cerr << "Error send file->" << strerror(errno) << std::endl;
            close(fileFd);
            return;
        }
        else if (bytes_sent == 0)
        {
            break;
        }
    }
    shutdown(dataFd, SHUT_WR);
    close(fileFd);
    LOG_DEBUG << "客户端发送完毕，关闭连接";
}
void sendFile::getConn(const mulib::net::TcpClient::TcpConnectionPtr conn){
    ftpConn = conn;
}
mulib::net::TcpClient::TcpConnectionPtr sendFile::returnConn(){
    return ftpConn;
}
int sendFile::createFile(std::string file){
    std::string fileName = file;
    std::filesystem::create_directories("/tmp/downloads/");
    chdir("/tmp/downloads");
    int fileFd = open(fileName.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fileFd < 0){
        LOG_ERROR << "create file failed:" << strerror(errno);
        sendMeg("550 Failed to open file for writing.");
        return -1;
    }
    return fileFd;
}
void sendFile::retr(int &fileFd, int &dataFd){
    char buffer[1024];
    int read_bytes = 0;
    LOG_DEBUG << "reading";
    while ((read_bytes = read(dataFd, buffer, sizeof(buffer))) > 0){
        if (write(fileFd, buffer, read_bytes) != read_bytes){
            LOG_DEBUG << "write fail" << strerror(errno);
            close(fileFd);
        }
    }
    if (read_bytes == 0){
        LOG_DEBUG << "read end";
        close(fileFd);
    }
    else{
        LOG_DEBUG << "read fail" << strerror(errno);
        close(fileFd);
    }
}
void sendFile::getmainConn(const mulib::net::TcpClient::TcpConnectionPtr mainconn){
    mainConn = mainconn;
}
void sendFile::sendmain(std::string fileName,std::string from, std::string to, std::string myname, std::string fileSize, mulib::base::Timestamp recviveTime){
    nlohmann::json j;
    recviveTime = recviveTime.now();
    j["type"] = "message";
    j["account"] = "user:" + from;
    std::string things = "[" + recviveTime.toFormattedString().substr(0, 16) + "-" + myname + "]:发送了文件:" + fileName;
    j["things"] = things;
    if (to.substr(0, 4) == "user"){
        j["receive"] = to;
    }
    else{
        j["group"] = to;
        j["type"] = "gmessage";
    }
    mainConn->send(MessageSplitter::encodeMessage(j.dump()));
    std::cout << "\r\33[2K"; // 清除整行
    std::cout << things << std::endl;
    std::cout << "\033[1;34m" << "> " <<  "\033[0m";
    std::cout << "\033[0C" << std::flush;
    nlohmann::json js;
    js["type"] = "file";
    js["fileName"] = fileName;
    if (to.substr(0, 4) == "user"){
        js["value"] = tool::swapsort("file:" + from, to, "file:");
    }
    else{
        js["value"] = "file:" + to.substr(5);
    }
    js["size"] = fileSize;
    mainConn->send(MessageSplitter::encodeMessage(js.dump()));
}