#include "GameChannel.h"
#include "GameMsg.h"

#include "msg.pb.h"

#include "AOIWorld.h"
#include "ZinxTimer.h"

#include "RandomName.h"
#include <sys/types.h>
#include <sys/stat.h>

//�� GameRole�� �õ� ������ �����������ʼ��
extern RandomName random_name;



//�ػ�����
void daemon_init()
{
	/*
	1��fork
	2���������˳�
	3���ӽ������ûػ� ID
	4���ӽ�������ָ��·��
	5���ӽ����ض��������ļ�������
	*/
	//1 fork
	int m_ipid = fork();
	if (0 > m_ipid)
	{
		//�쳣�˳�
		exit(-1);
	}
	if (0 < m_ipid)
	{
		//2 �������˳�
		exit(0);
	}
	
	//3 �ӽ��� ���ûỰID
	setsid();
	//4 �ӽ��� ���ù���·��
	chdir("/home/mgpdian/projects/gameProject/bin/x64/Debug");
	//5 �ӽ��� �ض���3���ļ������� /dev/null
	//�����web ����Թر����дӸ����̼̳еĲ�����Ҫ���ļ������� 
	//������������Ҫ�� ������Ҫ�ض���
	//for (int i = 0; i < NOFILE; close(i++));
	int nullfd = open("/dev/null", O_RDWR);
	if (nullfd >= 0)
	{
		dup2(nullfd, 0);
		dup2(nullfd, 1);
		dup2(nullfd, 2);
		close(nullfd);
	}
	
	//���������ܲ���Ҫ
	//6 ��������� umask(0)
	//umask(0);
	//7 ����SIGCHLD�ź�
	//signal(SIGCHLD, SIG_IGN);


	while (1)
	{
		int pid = fork();
		if (0 > pid)
		{
			//�쳣�˳�
			exit(-1);
		}

		//�����̵ȴ��ӽ����˳�
		if (0 < pid)
		{
			int iStatus = 0;
			wait(&iStatus);
			//�ж��Ƿ����쳣�˳�  

			//����������˳�  �������˳�
			if (0 == iStatus)
			{
				exit(0);
			}
		}
		//�ӽ�������ѭ��ִ����Ϸҵ��
		else {
			break;
		}
	}
}




////���Ե�ͼ��
//class myPlayer : public Player
//{
//public:
//	myPlayer(int _x, int _y, std::string _name) : x(_x), y(_y) , name(_name){}
//	int x;
//	int y;
//	std::string name;
//	// ͨ�� Player �̳�
//	virtual int GetX() override
//	{
//		return x;
//	}
//	virtual int GetY() override
//	{
//		return y;
//	}
//};


//��������
void test()
{
	//���� ��Ϣ������л�
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

	//���� ��Ϣ��ķ����л�
	//char buff[] = { 0x08, 0x01, 0x12, 0x04, 0x74, 0x65, 0x73, 0x74 };
	//std::string input(buff, sizeof(buff));

	//auto ingm = GameMsg(GameMsg::MSG_TYPE_LOGIN_ID_NAME, input);
	//std::cout << dynamic_cast<pb::SyncPid*>(ingm.getMsg())->pid() << std::endl;
	//std::cout << dynamic_cast<pb::SyncPid*>(ingm.getMsg())->username() << std::endl;


	////���� ��ͼ��
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

	//�ػ�����
	daemon_init();

	//��ʼ��������
	random_name.LoadFile();

	ZinxKernel::ZinxKernelInit();


	//��Ӽ���ͨ��
	ZinxKernel::Zinx_Add_Channel(*(new ZinxTCPListen(51111, new GameConnFact())));
	
	//��Ӷ�ʱ��ͨ��
	ZinxKernel::Zinx_Add_Channel(*(new ZinxTimerChannel()));

	
	//����
	ZinxKernel::Zinx_Run();

	ZinxKernel::ZinxKernelFini();

	return 0;
}


