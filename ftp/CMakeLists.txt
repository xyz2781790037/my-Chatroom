cmake_minimum_required(VERSION 3.13)
project(MychatroomProject)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 查找 JSON 库
find_package(nlohmann_json 3.2.0 REQUIRED)
set(FTP_HEADERS
    handleFile.h
    ../netlib/net/TcpServer.h
    ../base/threadPool.h
)
# 设置源文件
set(FTP_SOURCES
    server.cc
    handleFile.cc
)

# 创建可执行文件
add_executable(ftp ${FTP_SOURCES})

# 链接库
target_link_libraries(ftp
    PRIVATE
    netlib
    pthread
    nlohmann_json::nlohmann_json
)

# 包含头文件目录
target_include_directories(ftp PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/../base
)
