# 34 hiredis完成发布订阅

复习一下 hiredis的使用

```c++
auto pc = redisConnect("127.0.0.1", 50000);
//链接	
	if (nullptr != pc)
	{
		redisReply* red = (redisReply*)redisCommand(pc, "publish 1 abc");
        //使用 并获得返回值
		freeReplyObject(red); //释放返回值
		redisFree(pc);//释放链接
	}
```



hiredis 订阅功能实现

```c++
#include <iostream>
#include <stdlib.h>
int main()
{
	auto pc = redisConnect("127.0.0.1", 6379);
	
	if (nullptr != pc)
	{
		//redisReply* red = (redisReply*)redisCommand(pc, "publish 1 abc");
		redisReply* reply = (redisReply*)redisCommand(pc, "subscribe 1");
		
		if (REDIS_REPLY_ARRAY == reply->type)
		{
			std::cout << reply -> elements << std::endl;
			puts(reply->element[0] ->str);
			puts(reply->element[1]->str);
			
			std::cout << reply->element[2]->integer << std::endl;
		}

		//freeReplyObject(red);
		freeReplyObject(reply);
		redisFree(pc);
	}
	
	getchar();
	return 0;
}
```

```c++
3
subscribe
1
1
```

但是这样的话只能连接一次  不能接受到后面的通道传输过来的数据



使用阻塞 获取redis订阅回复

```c++
freeReplyObject((redisReply*)redisCommand(pc, "subscribe 1"));
		//接收redis发来的数据
		redisReply* reply = NULL;
		redisGetReply(pc, (void**)&reply); //redisGetReply会等待redis发来数据

		std::cout << "message from channel:" << std::string(reply->element[1]->str) << std::endl;
		std::cout << "message content:" << std::string(reply->element[2]->str) << std::endl;

```

缺点是 如果要获取的话 就会一直阻塞在那里 很浪费



### 用hiredis完成发布订阅

- 发布：redisCommand("publish XXXX XXX")

- 订阅（阻塞）：

  - redisCommand("subscribe XXXX")---》回复内容：订阅

    了

  - redisGetReply（redis回复结构体指针的指针）---》函数

- 会阻塞接收到的消息格式：回复的类型是数组，第0个元素"message",第1个元素是字符串类型的频道名；第2个元素是字符串类型的消息内容



