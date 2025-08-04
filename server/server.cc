#include "../netlib/net/TcpServer.h"
#include <nlohmann/json.hpp>
#include "handleData.h"
#include "../base/threadPool.h"
extern ConnectionManager connectionmanger_;
extern std::mutex timeMutex;
extern std::unordered_map<mulib::net::TcpConnectionPtr, time_t> lastActiveTime;
void onConnection(const mulib::net::TcpConnectionPtr &conn)
{
    if (conn->connected()){
        std::cout << "new conncetion\033[1;35m"<< conn << "\033[0m" << std::endl;
    }
}
void onMessage(const TcpConnectionPtr &conn, mulib::base::Timestamp recviveTime, std::shared_ptr<handleData> &handledata_, MessageSplitter &megser, redisCmd &redis)
{
    handledata_->Megcycle(conn, megser,redis,recviveTime);
}
int main(){
    ThreadPool pool(128);
    auto handledata_ = std::make_shared<handleData>();
    mulib::net::EventLoop mainLoop;
    mulib::net::InetAddress addr(8080);
    mulib::net::TcpServer server(&mainLoop, "GChat", addr);
    server.setThreadNum(32);
    server.setConnectionCallback([&mainLoop](const mulib::net::TcpConnectionPtr &conn){
        onConnection(conn);
        std::thread([&mainLoop]() {
            while (true) {
                std::this_thread::sleep_for(std::chrono::seconds(60));
                time_t now = std::time(nullptr);
                std::vector<mulib::net::TcpConnectionPtr> toClose;

                for (auto &[conn, lastTime] : lastActiveTime)
                {
                    if (now - lastTime > 60)
                    { // 超过60秒没消息
                        LOG_INFO << "连接 " << conn << " 超时，将关闭";
                        toClose.push_back(conn);
                    }
                    }
                    
                // 主线程中关闭连接（不能直接从线程里调用关闭）
                for (mulib::net::TcpConnectionPtr &oneConn : toClose) {
                    mainLoop.runInLoop([oneConn]() {
                        if (oneConn){
                            oneConn->shutdown();
                            oneConn->forceClose();
                        } 
                    });
                }
            }
        }).detach();
        if (!conn->connected()) {
            std::lock_guard<std::mutex> lock(timeMutex);
            lastActiveTime.erase(conn);
            LOG_INFO << "\033[1;38m连接 " << conn << " 已断开，清除心跳记录\033[0m";
            thread_local redisCmd redis;
            std::string account = connectionmanger_.getName(conn);
            if(account != "null"){
                LOG_INFO << account << "异常下线退出";
                redis.hset(account,"mystate","offline");
                connectionmanger_.removeUserConn(account, conn);
            }
        }
    });
    server.setMessageCallback([&handledata_, &pool](const TcpConnectionPtr &conn, Buffer *buf, mulib::base::Timestamp recviveTime) {
        std::string msg = buf->retrieveAllAsString();

        // pool.enqueue([conn, recviveTime, &handledata_, msg]
        //              {
            thread_local redisCmd redis;
            MessageSplitter megser;
            megser.append(msg);
            onMessage(conn,recviveTime,handledata_,megser,redis); 
        // });
     });
    server.start();
    mainLoop.loop(-1);
    return 0;
}