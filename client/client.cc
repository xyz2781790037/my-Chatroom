#include <../netlib/net/EventLoop.h>
#include "../netlib/net/TcpConnection.h"
#include "../base/logOn.h"
#include "handleMeg.h"
#include "../base/threadPool.h"
#include "TermiosGuard.h"
#include "sendFile.h"
int main(int argc, char *argv[])
{
    ThreadPool pool(16);
    TermiosGuard guard;
    // mulib::base::Logger::setLogLevel(mulib::base::Logger::DEBUG);
    auto mainLoop = std::make_shared<mulib::net::EventLoop>();
    mulib::net::InetAddress addr(argv[1], 8080);
    mulib::net::InetAddress addr2(argv[1], 8088);
    auto client1Ptr = std::make_shared<mulib::net::TcpClient>(mainLoop.get(), addr);
    auto client2Ptr = std::make_shared<mulib::net::TcpClient>(mainLoop.get(), addr2);
    sendFile sendFile_;
    logon clientLog(client1Ptr);
    client1Ptr->setConnectionCallback([&clientLog,&sendFile_](const mulib::net::TcpClient::TcpConnectionPtr &conn) {
    if (conn->connected()) {
        std::cout << "连接服务器成功，可以开始交互" << std::endl;
        sendFile_.getmainConn(conn);
        std::thread uiThread([&clientLog]() mutable
                             { clientLog.ui(); });
        uiThread.detach();
        // std::thread Tcp([conn]() mutable{ 
        //                     while(1){
        //                         std::this_thread::sleep_for(std::chrono::seconds(15));
        //                         nlohmann::json j;
        //                         j["type"] = "tcp";
        //                         conn->send(MessageSplitter::encodeMessage(j.dump()));
        //                     }
        //                 });
        // Tcp.detach();
    } else {
        std::cout << "连接断开" << std::endl;
    } });
    
    
    handleMeg handlemeg_;
    client2Ptr->setConnectionCallback([&sendFile_,&handlemeg_](const mulib::net::TcpClient::TcpConnectionPtr &conn)  {
        if (conn->connected()) {
            std::cout << "连接FTP成功，可以开始交互" << std::endl;
            sendFile_.getConn(conn);
            handlemeg_.getConn(conn);
        } else {
            std::cout << "FTP连接断开" << std::endl;
        }
     });
     client2Ptr->setMessageCallback([&sendFile_](const mulib::net::TcpClient::TcpConnectionPtr &conn, Buffer *buf, mulib::base::Timestamp recviveTime) {  
        LOG_DEBUG << "ftp收到消息";
        std::string msg = buf->retrieveAllAsString();
        MessageSplitter megSpl;
        megSpl.append(msg);
        sendFile_.recvMeg(megSpl,recviveTime);
    });
    client1Ptr->setMessageCallback([&handlemeg_,&pool](const mulib::net::TcpClient::TcpConnectionPtr &conn, Buffer *buf, mulib::base::Timestamp recviveTime) {
        LOG_DEBUG << "gchat收到消息";
        std::string buf_ = buf->retrieveAllAsString();
        pool.enqueue([conn,recviveTime,&handlemeg_,buf_]{
            MessageSplitter megSpl;
            megSpl.append(buf_);
            handlemeg_.recviveMeg(conn, recviveTime, megSpl);
        });
        
        
    });
    client2Ptr->connect();
    client1Ptr->connect();
    mainLoop->loop(-1);
    return 0;
}