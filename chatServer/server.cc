#include "../netlib/net/TcpServer.h"
#include <cpp_redis/cpp_redis>
void onConnection(const mulib::net::TcpConnectionPtr &conn)
{
    if (conn->connected())
    {
        std::cout << "A connection has came\n";
        conn->send("hello!");
    }
}
int main(){
    cpp_redis::client redisClient;
    redisClient.connect("127.0.0.1", 6379, [](const std::string &, size_t, cpp_redis::client::connect_state state)
                        {
        if (state == cpp_redis::client::connect_state::ok) {
            std::cout << "[Redis] Connected successfully.\n";
        } else {
            std::cout << "[Redis] Connection failed.\n";
        } });
    redisClient.sync_commit();
    mulib::net::EventLoop mainLoop;
    mulib::net::InetAddress addr(8080);
    mulib::net::TcpServer server(&mainLoop, "G", addr);
    server.setThreadNum(32);
    server.start();
    server.setConnectionCallback(onConnection);
    mainLoop.loop(-1);
    return 0;
}