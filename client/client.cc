#include "../base/logOn.h"
#include <../netlib/net/TcpClient.h>
#include <../netlib/net/EventLoop.h>
#include "../netlib/base/logger.h"
#include "../netlib/net/TcpConnection.h"
#include "../base/handleData.h"
#include "../base/MessageSplitter.h"
int main(){
    mulib::base::Logger::setLogLevel(mulib::base::Logger::DEBUG);
    mulib::net::EventLoop mainLoop;
    mulib::net::InetAddress addr("10.30.0.127", 8080);
    auto clientPtr = std::make_shared<mulib::net::TcpClient>(&mainLoop, addr);
    clientPtr->connect();
    handleData handledata_;
    logon clientLog(clientPtr);
    clientPtr->setConnectionCallback([&clientLog](const mulib::net::TcpClient::TcpConnectionPtr &conn)
                                 {
    if (conn->connected()) {
        std::cout << "连接成功，可以开始交互" << std::endl;
        std::thread uiThread([&clientLog]() mutable
                             { clientLog.ui(); });
        uiThread.detach();
    } else {
        std::cout << "连接断开" << std::endl;
    } });

    clientPtr->setMessageCallback([&clientLog, &handledata_](const mulib::net::TcpClient::TcpConnectionPtr &conn, Buffer *buf, mulib::base::Timestamp){
        MessageSplitter megSpl;
        megSpl.append(buf);
        std::string msg;
        while(megSpl.nextMessage(msg)){
            LOG_INFO << msg;
            auto jsonData = nlohmann::json::parse(msg);
            handleData::type type = handledata_.getDataType(jsonData["type"]);
            std::cout << type << std::endl;
            if (type == handleData::PRINT)
            {
                std::cout << "收到服务器消息：" << jsonData["meg"] << std::endl;
                LOG_INFO << "state: " << jsonData["state"];
                clientLog.updataState(jsonData["state"]);
            }
            else if(type == handleData::GETPWD){
                
            }
        }
    });
    mainLoop.loop(-1);
    return 0;
}