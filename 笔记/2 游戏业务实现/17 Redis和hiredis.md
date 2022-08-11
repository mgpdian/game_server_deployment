# 17 Redis 和 hiredis

具体的看  [这里](G:\笔记\redis)

后台启动服务器 redis-server /etc/redis.conf

服务器端口号 6379

客户端访问 redis-cli

## **redis命令**（redis-cli XXXX） 

- set key value: 存数据（value） 

- get key ：显示数据
- del key：删除一对数据
- lpush，rpush存链表节点
- lrange遍历
- lrem删除n个节点

# redis程序结构 

- cs结构，数据放在服务进程的内存中
- 命令行客户端连接本地或远程地址访问
- 多种API可以访问：hiredis
- 程序结构简单，内部的数据结构和算法优秀





# Hiredis

### 安装

[这里](G:\笔记\简单命令行聊天室\C++Redis连接.md)

头文件为

```c++
#include <hiredis/hiredis.h>
```



基本使用操作

```c++
//连接
redisContext *c= redisConnect("127.0.0.1", 6379);

//使用
reidsReply *r= (redisReply*)redisCommand(c, "PING");
//查看
puts(r -> str);

//释放查询的内容的内存
freeReplyObject(r)

//释放连接
redisFree(c);

```



总结

# hiredisAPI使用

- C函数库，包含头文件 <hiredis/hiredis.h> ,编译时指定链接参数为 -L/usr/local/lib -lhiredis 
- redisConnect跟数据库建立链接（redisFree释放掉） 
- redisCommand发命令并通过返回值取出结果（freeReplyObject释放掉） 
- 运行时若提示找不到共享库，则在.bashrc最末端添加一句 export LD_LIBRARY_PATH=/usr/local/lib ,重新打开终端运行 



项目的具体应用见[16章](G:\笔记\游戏服务器\笔记\2 游戏业务实现\16 存储当局玩家姓名及redis实现.md)