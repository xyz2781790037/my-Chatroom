#include <../base/logOn.h>
#include <../netlib/net/TcpClient.h>
#include <../netlib/net/EventLoop.h>
int main(){
    mulib::net::EventLoop mainLoop;
    mulib::net::InetAddress addr(8080);
    mulib::net::TcpClient client(&mainLoop,addr);
    logon clientLog;
    clientLog.ui(client);
    return 0;
}