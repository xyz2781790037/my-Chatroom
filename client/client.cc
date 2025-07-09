#include "../base/logOn.h"
#include <../netlib/net/TcpClient.h>
#include <../netlib/net/EventLoop.h>
int main(){
    mulib::net::EventLoop mainLoop;
    mulib::net::InetAddress addr("10.30.0.127", 8080);
    mulib::net::TcpClient client(&mainLoop,addr);
    
    logon clientLog(client);
    client.connect();
    clientLog.ui();
    
    return 0;
}