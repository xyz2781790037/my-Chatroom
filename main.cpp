#include <cpp_redis/cpp_redis>

int main()
{
    cpp_redis::client client;

    client.connect("127.0.0.1", 6379,
                   [](const std::string &host, std::size_t port, cpp_redis::client::connect_state status)
                   {
                       if (status == cpp_redis::client::connect_state::dropped)
                       {
                           std::cerr << "client disconnected from " << host << ":" << port << std::endl;
                       }
                   });

    client.set("hello", "world");
    client.get("hello", [](cpp_redis::reply &reply)
               {
    if (reply.is_string()) {
      std::cout << "GET hello: " << reply.as_string() << std::endl;
    } });

    client.sync_commit();
    return 0;
}
