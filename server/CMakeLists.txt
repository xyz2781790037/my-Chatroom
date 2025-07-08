cmake_minimum_required(VERSION 3.13)
project(MyCppRedisProject)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 包含头文件
include_directories(/usr/local/include)

# 链接库路径
link_directories(/usr/local/lib)

# 添加可执行文件
set(SERVER_SOURCES
    server.cc

)
add_executable(server ${SERVER_SOURCES})
# 链接 cpp_redis 和 tacopie
target_link_libraries(server
    PRIVATE
    netlib
    cpp_redis
    tacopie
    pthread
)
target_include_directories(server PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)