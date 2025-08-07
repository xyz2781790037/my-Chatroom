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
    signal(SIGPIPE, SIG_IGN);
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
        LOG_DEBUG << "连接服务器成功，可以开始交互";
        sendFile_.getmainConn(conn);
        std::thread uiThread([&clientLog]() mutable
                             { clientLog.ui(); });
        uiThread.detach();
        std::thread Tcp([conn]() mutable{ 
                            while(1){
                                std::this_thread::sleep_for(std::chrono::seconds(15));
                                nlohmann::json j;
                                j["type"] = "tcp";
                                std::string a = j.dump();
                                conn->getLoop()->runInLoop([conn, &a]() {
                                    conn->send(MessageSplitter::encodeMessage(a));
                                });
                                
                            }
                        });
        Tcp.detach();
    } else {
        std::cout << "连接断开" << std::endl;
    } });
    
    
    handleMeg handlemeg_;
    client2Ptr->setConnectionCallback([&sendFile_,&handlemeg_](const mulib::net::TcpClient::TcpConnectionPtr &conn)  {
        if (conn->connected()) {
            LOG_DEBUG << "连接FTP成功，可以开始交互";
            sendFile_.getConn(conn);
            handlemeg_.getConn(conn);
        } else {
            std::cout << "FTP连接断开" << std::endl;
        }
     });
     MessageSplitter megSpl1;
     client2Ptr->setMessageCallback([&sendFile_,&megSpl1,&pool](const mulib::net::TcpClient::TcpConnectionPtr &conn, Buffer *buf, mulib::base::Timestamp recviveTime) {  
        LOG_DEBUG << "ftp收到消息";
        std::string msg1 = buf->retrieveAllAsString();
        
        megSpl1.append(msg1);
        std::vector<std::string> messages;
        std::string msg;
        while (megSpl1.nextMessage(msg)) {
            messages.push_back(msg);
        }
        pool.enqueue([messages = std::move(messages),&sendFile_,recviveTime]{
            for(const auto &item : messages){
                sendFile_.recvMeg(item,recviveTime);
            }
            
        });
            
    });MessageSplitter megSpl;
    client1Ptr->setMessageCallback([&handlemeg_, &pool, &megSpl](const mulib::net::TcpClient::TcpConnectionPtr &conn, Buffer *buf, Timestamp recviveTime) {
        LOG_DEBUG << "gchat收到消息";
        std::string buf_ = buf->retrieveAllAsString();
        megSpl.append(buf_);
    
        // 主线程中先拆好包
        std::vector<std::string> messages;
        std::string msg;
        while (megSpl.nextMessage(msg)) {
            messages.push_back(msg);
        }
    
        // 拷贝消息进线程池处理（避免跨线程访问 megSpl）
        pool.enqueue([conn, recviveTime, &handlemeg_, messages = std::move(messages)] {
            for (const auto& msg : messages) {
                handlemeg_.recviveMeg(conn, recviveTime, msg);
            }
        });
    });
    client2Ptr->connect();
    client1Ptr->connect();
    mainLoop->loop(-1);
    return 0;
}