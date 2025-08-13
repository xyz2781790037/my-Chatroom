# my-Chatroom
### 实现时间
#### 7.8 - 7.13 实现登陆注册
> 1. *通过muduo构建了服务端，客户端。*
> 2. *配置了Cmake-C++17,使用Cmake编译*
> 3. *配置了cpp_redis和tacopie，json,libcurl,netlib(将muduo写成静态库)*
> 4. *实现了登陆UI,用户注册，登录，退出*
> 5. *实现了通过邮箱验证码找回密码*
> 6. *实现了验证注册验证码的真实性*
#### 7.14 - 7.21 实现了用户间聊天
> 1. *实现了用户UI*
> 2. *实现了自我信息的查看，修改密码，用户名*
> 3. *实现了加好友，删除好友*
> 4. *实现了查看在线状态*
> 5. *实现在线聊天*
> 6. *实现屏蔽ctrl + d*
> 7. *实现离线消息存储*
> 8. *实现输入关键信息终端不回显*
> 9. *实现提醒用户消息和历史消息的储存*
> 10. *实现拉黑好友和非好友之间禁止聊天*
> 11. *实现消息的主界面通知*
#### 7.21 - 7.23 实现了群聊
> 1. *实现创建群聊和解散，退出群聊*
> 2. *实现主动加入群聊和被动邀请进群*
> 3. *实现查看自己加入和创建的群聊*
> 4. *实现群聊的在线聊天*
> 5. *实现存储群聊的离线和历史消息*
> 6. *实现设置管理和撤销管理*
> 7. *实现验证信息的完善*
> 8. *实现了查看群成员*
#### 7.24 - 7.29 实现文件系统
> 1. *构建muduo的ftp服务端*
> 2. *实现了文件的上传*
> 3. *实现了文件的下载*
> 4. *完善了发送效果的美观性*
> 5. *完善文件上传下载的逻辑*
> 6. *实现列出文件列表*
> 7. *实现了TCP心跳检测*
#### 7.30 - 8.7 完善聊天室
> 1. *完善加好友体系*
> 2. *注册后自动添加自己和不能拉黑删除自己*
> 3. *ctrl + c退出后自动退出登陆*
> 4. *简化文件系统且输入路径改用readline()*
> 5. *简化验证的逻辑，改指令为输入*
> 6. *修改了拉黑的逻辑，增加黑名单*
> 7. *修改收消息逻辑，消除了半包的影响*
> 8. *优化发消息逻辑，增加发消息速度*
> 9. *修改进入聊天加载100条历史消息*

### 环境准备
Linux系统（推荐 Ubuntu 等）
#### 客户端需要配置

#####  安装readline
```
sudo apt install libreadline-dev
```
#####  安装json
```
sudo apt update
sudo apt install nlohmann-json-dev
```

#### 服务端需要配置cpp_redis
##### 安装cpp_redis
- 安装依赖
```
sudo apt update
sudo apt install cmake g++ make libssl-dev
```
- 安装库
``` 
git clone https://github.com/Cylix/cpp_redis.git
cd cpp_redis
git submodule update --init --recursive  # 拉取 tacopie 子模块
```
- 编译和安装
```
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j32
sudo make install
```
##### CURL::libcurl
```
sudo apt update
sudo apt install libcurl4-openssl-dev
```
### 运行
#### 运行服务器
```
cd build/server
./server
```
#### 运行ftp
```
cd build/ftp
./ftp
```
#### 启用客户端
```
cd build/client
./client [ip]
```