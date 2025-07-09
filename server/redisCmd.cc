#include <redisCmd.h>
redisCmd::redisCmd(){
    redisClient.connect("127.0.0.1", 6379, [](const std::string &, size_t, cpp_redis::client::connect_state state)
                        {
        if (state == cpp_redis::client::connect_state::ok) {
            std::cout << "[Redis] Connected successfully.\n";
        } else {
            std::cout << "[Redis] Connection failed.\n";
        } });
    redisClient.sync_commit();
}
void redisCmd::assignId(){
    cpp_redis::client redisClient;
    redisClient.incr("user_id_counter", [](cpp_redis::reply &reply){
    if (reply.is_integer()) {
        int id = reply.as_integer();  // 得到新用户的唯一 id
    } });
    redisClient.sync_commit();
}