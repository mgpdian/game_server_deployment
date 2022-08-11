#include "GameChannel.h"
#include "GameMsg.h"

#include "msg.pb.h"

#include "AOIWorld.h"
#include "ZinxTimer.h"

#include "RandomName.h"
#include <sys/types.h>
#include <sys/stat.h>

//从 GameRole中 拿到 姓名池 在这里帮他初始化
extern RandomName random_name;



//守护进程
void daemon_init()
{
	/*
	1、fork
	2、父进程退出
	3、子进程设置回话 ID
	4、子进程设置指向路径
	5、子进程重定向三个文件描述符
	*/
	//1 fork
	int m_ipid = fork();
	if (0 > m_ipid)
	{
		//异常退出
		exit(-1);
	}
	if (0 < m_ipid)
	{
		//2 父进程退出
		exit(0);
	}
	
	//3 子进程 设置会话ID
	setsid();
	//4 子进程 设置工作路径
	chdir("/home/mgpdian/projects/gameProject/bin/x64/Debug");
	//5 子进程 重定向3个文件描述符 /dev/null
	//如果是web 则可以关闭所有从父进程继承的不再需要的文件描述符 
	//但是这里我们要看 所以需要重定向
	//for (int i = 0; i < NOFILE; close(i++));
	int nullfd = open("/dev/null", O_RDWR);
	if (nullfd >= 0)
	{
		dup2(nullfd, 0);
		dup2(nullfd, 1);
		dup2(nullfd, 2);
		close(nullfd);
	}
	
	//这两个可能不需要
	//6 清空屏蔽字 umask(0)
	//umask(0);
	//7 屏蔽SIGCHLD信号
	//signal(SIGCHLD, SIG_IGN);


	while (1)
	{
		int pid = fork();
		if (0 > pid)
		{
			//异常退出
			exit(-1);
		}

		//父进程等待子进程退出
		if (0 < pid)
		{
			int iStatus = 0;
			wait(&iStatus);
			//判断是否是异常退出  

			//如果是正常退出  则正常退出
			if (0 == iStatus)
			{
				exit(0);
			}
		}
		//子进程跳出循环执行游戏业务
		else {
			break;
		}
	}
}




////测试地图类
//class myPlayer : public Player
//{
//public:
//	myPlayer(int _x, int _y, std::string _name) : x(_x), y(_y) , name(_name){}
//	int x;
//	int y;
//	std::string name;
//	// 通过 Player 继承
//	virtual int GetX() override
//	{
//		return x;
//	}
//	virtual int GetY() override
//	{
//		return y;
//	}
//};


//测试用例
void test()
{
	//测试 消息类的序列化
	//pb::SyncPid* pmsg = new pb::SyncPid();
	//pmsg->set_pid(1);
	//pmsg->set_username("test");

	//GameMsg gm(GameMsg::MSG_TYPE_LOGIN_ID_NAME, pmsg);
	//auto output = gm.serialize();

	//for (auto byte : output)
	//{
	//	printf("%02X", byte);
	//}
	//puts("");

	//测试 消息类的反序列化
	//char buff[] = { 0x08, 0x01, 0x12, 0x04, 0x74, 0x65, 0x73, 0x74 };
	//std::string input(buff, sizeof(buff));

	//auto ingm = GameMsg(GameMsg::MSG_TYPE_LOGIN_ID_NAME, input);
	//std::cout << dynamic_cast<pb::SyncPid*>(ingm.getMsg())->pid() << std::endl;
	//std::cout << dynamic_cast<pb::SyncPid*>(ingm.getMsg())->username() << std::endl;


	////测试 地图类
	//AOIWorld w(20, 200, 50, 230, 6, 6);

	//myPlayer p1(60, 107, "1");
	//myPlayer p2(91, 118, "2");
	//myPlayer p3(147, 133, "3");

	//w.AddPlayer(&p1);
	//w.AddPlayer(&p2);
	//w.AddPlayer(&p3);

	//auto srd_list = w.GetSrdPlayers(&p1);
	//for (auto& single : srd_list)
	//{
	//	//dynamic_cast<myPlayer*> (single);
	//	std:d:cout << dynamic_cast<myPlayer*> (single) -> name << std::endl;
	//}
}


int main()
{
	//test();

	//守护进程
	daemon_init();

	//初始化姓名池
	random_name.LoadFile();

	ZinxKernel::ZinxKernelInit();


	//添加监听通道
	ZinxKernel::Zinx_Add_Channel(*(new ZinxTCPListen(51111, new GameConnFact())));
	
	//添加定时器通道
	ZinxKernel::Zinx_Add_Channel(*(new ZinxTimerChannel()));

	
	//运行
	ZinxKernel::Zinx_Run();

	ZinxKernel::ZinxKernelFini();

	return 0;
}


