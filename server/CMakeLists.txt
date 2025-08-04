cmake_minimum_required(VERSION 3.13)
project(MychatroomProject)
find_package(CURL REQUIRED)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 包含头文件
include_directories(/usr/local/include)

# 链接库路径
link_directories(/usr/local/lib)
# 查找库
find_package(nlohmann_json 3.2.0 REQUIRED)
# 添加可执行文件
set(SERVER_HEADERS
    handleData.h
    redisCmd.h
    ../base/MegType.h
    ../base/tool.h
    ../base/threadPool.h
    ConnectionManager.h
)
set(SERVER_SOURCES
    server.cc
    handleData.cc
    redisCmd.cc
    ../base/MegType.cc
    ../base/tool.cc
    ConnectionManager.cc
)
add_executable(server ${SERVER_SOURCES})
# 链接 cpp_redis 和 tacopie
target_link_libraries(server
    PRIVATE
    netlib
    cpp_redis
    tacopie
    pthread
    nlohmann_json::nlohmann_json
    CURL::libcurl
)
target_include_directories(server PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/../base
)