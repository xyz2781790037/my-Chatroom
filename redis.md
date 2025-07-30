✅ 1. 初始化客户端

cpp_redis::client client;
client.connect();

你也可以使用远程连接：

client.connect("127.0.0.1", 6379, [](const std::string& host, std::size_t port, cpp_redis::client::connect_state status) {
    if (status == cpp_redis::client::connect_state::dropped) {
        std::cout << "Client disconnected from " << host << ":" << port << std::endl;
    }
});

✅ 2. 常用 Redis 命令函数
🔹 SET 和 GET

client.set("key", "value");
client.get("key", [](cpp_redis::reply& reply) {
    if (reply.is_string()) {
        std::cout << "GET: " << reply.as_string() << std::endl;
    }
});
client.sync_commit(); // 提交所有指令（同步执行）

🔹 DEL（删除键）

client.del({"key1", "key2"});
client.sync_commit();

🔹 INCR / DECR（数值递增/递减）

client.incr("counter");
client.sync_commit();

🔹 EXISTS（判断键是否存在）

client.exists({"some_key"}, [](cpp_redis::reply& reply) {
    std::cout << "EXISTS: " << reply.as_integer() << std::endl; // 1 if exists
});
client.sync_commit();

🔹 LPUSH / RPUSH / LPOP / RPOP（列表操作）

client.lpush("mylist", {"a", "b", "c"});
client.rpop("mylist", [](cpp_redis::reply& reply) {
    std::cout << "RPOP: " << reply.as_string() << std::endl;
});
client.sync_commit();

🔹 HSET / HGET / HDEL（哈希表）

client.hset("myhash", "field1", "value1");
client.hget("myhash", "field1", [](cpp_redis::reply& reply) {
    std::cout << "HGET: " << reply.as_string() << std::endl;
});
client.sync_commit();

🔹 SADD / SMEMBERS（集合）

client.sadd("myset", {"a", "b", "c"});
client.smembers("myset", [](cpp_redis::reply& reply) {
    for (const auto& v : reply.as_array()) {
        std::cout << "SET member: " << v.as_string() << std::endl;
    }
});
client.sync_commit();

✅ 3. 事务支持（MULTI/EXEC）

client.multi();
client.set("key", "value");
client.get("key");
client.exec([](cpp_redis::reply& reply) {
    auto replies = reply.as_array();
    for (const auto& r : replies) {
        std::cout << "TX Reply: " << r << std::endl;
    }
});
client.sync_commit();

✅ 4. 发布/订阅
订阅频道

cpp_redis::subscriber sub;
sub.connect();
sub.subscribe("mychannel", [](const std::string& chan, const std::string& msg) {
    std::cout << "Received from " << chan << ": " << msg << std::endl;
});
sub.commit(); // 必须 commit 来使订阅生效

发布消息

client.publish("mychannel", "hello");
client.sync_commit();

✅ 5. pipeline 异步批量执行（默认行为）

client.set("key1", "val1");
client.set("key2", "val2");
client.get("key1", [](cpp_redis::reply& reply) {
    std::cout << "key1: " << reply.as_string() << std::endl;
});
client.sync_commit();  // 一次性提交执行所有命令