cmake_minimum_required(VERSION 3.13)
project(MyCppRedisProject)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 包含头文件
include_directories(/usr/local/include)

# 链接库路径
link_directories(/usr/local/lib)

# 添加可执行文件
include_directories(
    ${CMAKE_SOURCE_DIR}/server
    ${CMAKE_SOURCE_DIR}/netlib
    ${CMAKE_SOURCE_DIR}/client
)
add_subdirectory(netlib)
add_subdirectory(server)
add_subdirectory(client)