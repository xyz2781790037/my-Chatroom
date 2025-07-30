cmake_minimum_required(VERSION 3.13)
project(MychatroomProject)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 包含头文件
include_directories(/usr/local/include)

# 链接库路径
link_directories(/usr/local/lib)
# 查找库
find_package(nlohmann_json 3.2.0 REQUIRED)
# 添加可执行文件
set(CLIENT_HEADERS
    ../base/logOn.h
    ../base/MegType.h
    handleMeg.h
    UserUi.h
    ../base/user.h
    ../base/tool.h
    MessageManager.h
    TermiosGuard.h
    sendFile.h
)
set(CLIENT_SOURCES
    client.cc
    UserUi.cc
    ../base/MegType.cc
    ../base/tool.cc
    MessageManager.cc
    sendFile.cc
)
add_executable(client ${CLIENT_SOURCES})
# 链接 cpp_redis 和 tacopie
target_link_libraries(client
    netlib
    nlohmann_json::nlohmann_json
    readline
)
target_include_directories(client PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/../base
    ${CMAKE_SOURCE_DIR}/../server
)