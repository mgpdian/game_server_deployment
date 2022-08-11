#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "user_opt.h"
#include <string>
#include <hiredis/hiredis.h>
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TBufferTransports.h>
#include "gen-cpp/container_mng.h"
void AddUser(const char *_name, const char *_password){
	char buff[1024] = {0};

	sprintf(buff, "./add_user.sh %s %s", _name, _password);
	system(buff); //脚本
}
bool CheckUser(const char *_name, const char *_password_dgst)
{
	bool bRet = false;

	//父进程 观察子进程是否运行成功
	if(fork() > 0)
	{

		int iStatus = 0;
		wait(&iStatus);
		if(0 == iStatus)
		{
			bRet = true;
		}
	}
	else{
		//子进程
		execlp("./check_user.sh", "./check_user.sh", _name, _password_dgst, NULL);
		//第一个参数"./check_user.sh" 类似于this
	}
	return bRet;
}

static int g_cur_select = 0;
std::string create_container(int _room_no)
{
	//return "x.x.x.x xxxxx"
	//1 确定调用在哪个服务器上创建
	//std::string port;
	std::string server_array[2] = {"192.168.81.128", "192.168.81.129"};
	std::string cur_server = server_array[g_cur_select++];
	g_cur_select %= sizeof(server_array)/sizeof(server_array[0]);

	std::shared_ptr<apache::thrift::transport::TTransport> socket(new apache::thrift::transport::TSocket(cur_server.c_str(), 9090));
	std::shared_ptr<apache::thrift::protocol::TProtocol> protocol(new apache::thrift::protocol::TBinaryProtocol(socket));

	container_mngClient client(protocol);

	socket->open();
	int port = client.create_room(_room_no);

#if 0	
	//2 向这个服务器发送创建容器的请求
	//向这个服务器的redis(这个redis订阅着主服务器的redis)  发布一个消息
	//通道名字为 create_room
	// redis-cli publish create_room 
	redisContext* pc = redisConnect("127.0.0.1", 6379);
	if(nullptr != pc)
	{
		freeReplyObject(redisCommand(pc, "publish create_room %s:%d", cur_server.c_str(), _room_no));
		//3 等待服务器返回创建容器的端口
		freeReplyObject(redisCommand(pc, "subscribe server_port"));

		redisReply *reply = NULL;
		if(REDIS_OK == redisGetReply(pc, (void**)&reply))
		{
			port.append(reply->element[2]->str);
			freeReplyObject(reply);
		}
		redisFree(pc);
	}

	//4 return IP + PORT
	
	//std::string ret;
	// //popen 可以运行脚本 并且获取他的输出
	// FILE* fd = popen("./create_room.sh", "r");
	// if(nullptr != fd)
	// {
	// 	char buff[1024];
	// 	fread(buff, 1, sizeof(buff), fd);
	// 	ret.append(buff);
	// 	//退出最后一个字符 --\n
	// 	ret.pop_back();
	// 	pclose(fd);
	// }
	//return ret;
#endif
	return cur_server + " " + std::to_string(port);
}

std::string check_room(const char *_room_no)
{
	//return "x.x.x.x xxxxx"
	std::string ret;

	redisContext* pc = redisConnect("127.0.0.1", 6379);

	if(nullptr != pc)
	{
		redisReply* reply = (redisReply*)redisCommand(pc, "get room_no%s", _room_no);
		if(nullptr != reply->str)
		{
			ret.append(reply->str);
		}
		freeReplyObject(reply);
		redisFree(pc);
	}

	return ret;
	/*
	bool bRet = false;

	if(fork() >= 0)
	{
		int iStatus = 0;
		wait(&iStatus);
		if (0 == iStatus)
		{
			bRet = true;
		}

	}
	else{
		execlp("./check_room.sh", "./check_room.sh", _room_no, NULL);
	}
	return bRet;
	*/
}
