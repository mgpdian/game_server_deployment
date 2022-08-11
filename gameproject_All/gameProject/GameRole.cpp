#include "GameRole.h"
#include "GameMsg.h"
#include <iostream>
#include "AOIWorld.h"
#include "msg.pb.h"
#include "GameProtocol.h"
#include "GameChannel.h"
#include <algorithm>
#include <random>
#include "ZinxTimer.h"
#include "RandomName.h"
#include <fstream>
#include <hiredis/hiredis.h>
//������Ϸ����ȫ�ֵ�������
static AOIWorld world(0, 400, 0, 400, 20, 20);

//����������
 RandomName random_name;

//��װ id �� name ���ڵ�¼   1
GameMsg* GameRole::CreateIDNameLogin()
{

    pb::SyncPid* pMsg = new pb::SyncPid();
    pMsg->set_pid(m_iPid);
    pMsg->set_username(m_szName);

    GameMsg* pRet = new GameMsg(GameMsg::MSG_TYPE_LOGIN_ID_NAME, pMsg);
    return pRet;
    
}

//��װ��Χ ��ҵ�λ�� 202
GameMsg* GameRole::CreateSrdPlayers()
{

    pb::SyncPlayers* pMsg = new pb::SyncPlayers();
    
    //��Χ�����
    std::list<Player*> srd_list = world.GetSrdPlayers(this);

    for (auto single : srd_list)
    {
        auto pPlayer = pMsg->add_ps();
        auto pRole = dynamic_cast<GameRole*>(single);
        pPlayer->set_pid(pRole->m_iPid);
        pPlayer->set_username(pRole->m_szName);
        
        //λ������
        auto pPostion = pPlayer->mutable_p();
        pPostion->set_x(pRole->x);
        pPostion->set_y(pRole->y);
        pPostion->set_z(pRole->z);
        pPostion->set_v(pRole->v);

    }

    
    GameMsg* pRet = new GameMsg(GameMsg::MSG_TYPE_SRD_POSITION, pMsg);
    return pRet;
}

//��װ�Լ���λ�� �� ��Χ����� 200
GameMsg* GameRole::CreateSelfPostion()
{
    pb::BroadCast* pMsg = new pb::BroadCast();
    //���Ϊ�㲥�� ���Ϊ200
    //���ID����������/��ʼλ��/������Ԥ����/��λ��
    pMsg->set_pid(m_iPid);
    pMsg->set_username(m_szName);

    //����λ����Ϣ ��Ŷ� ��ʼλ��
    pMsg->set_tp(2);

    auto pPostition = pMsg->mutable_p();
    pPostition->set_x(x);
    pPostition->set_y(y);
    pPostition->set_z(z);
    pPostition->set_v(v);


    GameMsg* pRet = new GameMsg(GameMsg::MSG_TYPE_BROADCAST, pMsg);
    return pRet;
}
 
//��װ id �� name  �������� 201
GameMsg* GameRole::CreateIDNameLogoff()
{
    pb::SyncPid* pMsg = new pb::SyncPid();
    pMsg->set_pid(m_iPid);
    pMsg->set_username(m_szName);

    //�� ���ߵ���Ϣ��ͬ���� ������ϢIDΪ201
    GameMsg* pRet = new GameMsg(GameMsg::MSG_TYPE_LOGOFF_ID_NAME, pMsg);
    return pRet;
}


//��װ��ҵ�������Ϣ ���͸�������� 200
GameMsg* GameRole::CreateTalkBroadCast(std::string _content)
{
    pb::BroadCast* pMsg = new pb::BroadCast();
    //���Ϊ�㲥�� ���Ϊ200
    //���ID����������/��ʼλ��/������Ԥ����/��λ��
    pMsg->set_pid(m_iPid);
    pMsg->set_username(m_szName);

    //����������Ϣ ���һ ��������
    pMsg->set_tp(1);

    pMsg->set_content(_content);

    GameMsg* pRet = new GameMsg(GameMsg::MSG_TYPE_BROADCAST, pMsg);

    return pRet;
}

static std::default_random_engine random_engine(time(NULL));

GameRole::GameRole()
{
    ////�������IDΪ��ǰ���ӵ�fd
    //m_iPid = m_pProto->getGameChannel()->GetFd();
    this->m_szName = random_name.GetName();
    x = 100 + random_engine() % 50;
    z = 100 + random_engine() % 50;
}

GameRole::~GameRole()
{
    random_name.Release(this->m_szName);
}


void GameRole::setProtocol(GameProtocol* _pProto)
{
    this->m_pProto = _pProto;
}

GameProtocol* GameRole::getProtocol()
{
    return this -> m_pProto;
}
//����������Ϣ
void GameRole::ProcTalkMsg(std::string _content)
{
    //��ȡ�������
    auto role_list = ZinxKernel::Zinx_GetAllRole();

    GameMsg* pmsg;
    for (auto& pRole : role_list)
    {
        pmsg = CreateTalkBroadCast(_content);
        auto pGameRole = dynamic_cast<GameRole*>(pRole);
        ZinxKernel::Zinx_SendOut(*pmsg, *(pGameRole->m_pProto));
    }
}
//����Χ��ҷ���λ��
void GameRole::ProcMoveMsg(int _x, int _y, int _z, int _v)
{
    //1 ��������
    // ��ȡԭ���ھ�s1
    std::list<Player*> srd_list_old = world.GetSrdPlayers(this);
    // �������� ժ��������, ��������� �õ��µ��ھ�s2
    world.DelPlayer(this);
    x = _x;
    y = _y;
    z = _z;
    v = _v;
    world.AddPlayer(this);

    std::list<Player*> srd_list_new = world.GetSrdPlayers(this);
    // ����s2 ��Ԫ�ز�����s1 ��Ұ�г���
    for (auto& single_player : srd_list_new)
    {
        if (srd_list_old.end() == find(srd_list_old.begin(), srd_list_old.end(), single_player))
        {
            //��Ұ�г���
            ViewAppear(dynamic_cast<GameRole*>(single_player));
        }
    }
    // ����s1 ��Ԫ�ز�����s2  ��Ұ����ʧ
    for (auto &single_player : srd_list_old)
    {
        if (srd_list_new.end() == find(srd_list_new.begin(), srd_list_new.end(), single_player))
        {
            //��Ұ��ʧ
            ViewLost(dynamic_cast<GameRole*>(single_player));
        }
    }
    //2�㲥��λ�ø���Χ��� 
    
    std::list<Player*> srd_list = world.GetSrdPlayers(this);
    for (auto& single : srd_list)
    {
        //��ɴ����͵ı���
        pb::BroadCast* pMsg = new pb::BroadCast();
        auto pPos = pMsg->mutable_p();

        pPos->set_x(_x);
        pPos->set_y(_y);
        pPos->set_z(_z);
        pPos->set_v(_v);

        pMsg->set_pid(m_iPid);
        pMsg->set_username(m_szName);
        pMsg->set_tp(4);

        auto pRole = dynamic_cast<GameRole*>(single);
        ZinxKernel::Zinx_SendOut(*(new GameMsg(GameMsg::MSG_TYPE_BROADCAST, pMsg)), *(pRole->m_pProto));
    }
}
//�� �����µ������Ұ��Χ  ��Ұ����
void GameRole::ViewAppear(GameRole* _pRole)
{
    //���Լ����Ͳ�����ҵ�200��Ϣ
    GameMsg* pmsg = _pRole->CreateSelfPostion();
    ZinxKernel::Zinx_SendOut(*pmsg, *this->m_pProto);
    
    //�������ҷ����Լ���202��Ϣ
    pmsg = this->CreateSelfPostion();
    ZinxKernel::Zinx_SendOut(*pmsg, *(_pRole->m_pProto));
}
//�� �뿪�ɵ������Ұ��Χ  ��Ұ��ʧ
void GameRole::ViewLost(GameRole* _pRole)
{
    //���Լ����Ͳ�����ҵ�201��Ϣ
    GameMsg* pmsg = _pRole->CreateIDNameLogoff();
    ZinxKernel::Zinx_SendOut(*pmsg, *this->m_pProto);

    //�������ҷ����Լ���201��Ϣ
    pmsg = this->CreateIDNameLogoff();
    ZinxKernel::Zinx_SendOut(*pmsg, *(_pRole->m_pProto));
}

//�˳���ʱ��������
class ExitTimer : public TimerOutProc {
    // ͨ�� TimerOutProc �̳�
    virtual void Proc() override
    {
        ZinxKernel::Zinx_Exit();
    }
    virtual int GetTimeSec() override
    {
        return 20;
    }
};
//ȫ�ֶ�ʱ��
static ExitTimer g_exit_timer;


bool GameRole::Init()
{
    if (ZinxKernel::Zinx_GetAllRole().size() <= 0)
    {
        TimerOutMng::GetInstance().DelTask(&g_exit_timer);
    }


    //����Լ�����Ϸ����
    bool bRet = false;
    //�������IDΪ��ǰ���ӵ�fd
    //Ϊʲôд�ڹ�������ᱨ��  ��Ϊ �����ʱ�� û�а�GameProtocol ��Ȼ�ᱨ����
    m_iPid = this->m_pProto->getGameChannel()->GetFd();

    bRet = world.AddPlayer(this);

    
    
    if (true == bRet)
    {
        //���Լ�����ID������
        GameMsg* pmsg = CreateIDNameLogin();
        //Zinx_SendOut�������� ֱ�ӽ����ݷ��͸�Э���
        ZinxKernel::Zinx_SendOut(*pmsg,*this->m_pProto);

        //���Լ�������Χ��ҵ�λ��
        pmsg = CreateSrdPlayers();
        ZinxKernel::Zinx_SendOut(*pmsg, *this->m_pProto);

        //����Χ��ҷ����Լ���λ��
        
        //��Χ�����
        std::list<Player*> srd_list = world.GetSrdPlayers(this);
        for (auto& single : srd_list)
        {
            pmsg = CreateSelfPostion();
            auto pRole = dynamic_cast<GameRole*>(single);
            ZinxKernel::Zinx_SendOut(*pmsg, *(pRole->m_pProto));

        }
    }

    //��׷�ӵ���ʽ �� ��¼��ǰ�������ļ�
   /* std::ofstream name_record("/tmp/name_record", std::ios::app);
    name_record << m_szName << std::endl;*/

    //��¼��ǰ���ֵ�redis��game_name��
    //1 ����redis
    
    redisContext* context = redisConnect("127.0.0.1", 6379);

    //2 ����lpush����
    if (nullptr != context)
    {
        redisReply* reply = (redisReply*)redisCommand(context, "lpush game_name %s", m_szName.c_str());

        //�ͷŲ�ѯ�����ݵ��ڴ�
        freeReplyObject(reply);

        //�ͷ�����
        redisFree(context);
    }


    return bRet;
}

//������Ϸ��ص��û�����
UserData* GameRole::ProcMsg(UserData& _poUserData)
{
    //����: ��ӡ��Ϣ����
    GET_REF2DATA(MultiMsg, input, _poUserData);

    for (auto& single : input.m_Msgs)
    {
        std::cout << "type is " << single->getMsgType() << std::endl;
        std::cout  << single->getMsg()->Utf8DebugString() << std::endl;

        
        //��������ѡ����
        switch (single->getMsgType())
        {
            //�жϳ� �������� ���͸��������
        case GameMsg::MSG_TYPE_CHAT_CONTENT:{
            //ȡ����������
            std::string content = dynamic_cast<pb::Talk*>(single->getMsg())->content();
            ProcTalkMsg(content);
           
            break;
            }
        case GameMsg::MSG_TYPE_NEW_POSITION :{
             // //1 ��������
            //2�㲥��λ�ø���Χ���    
                
                
            //ȡ����λ��
            pb::Position* NewPos = dynamic_cast<pb::Position*>(single->getMsg());
            //������Χ��� ����
            ProcMoveMsg(NewPos->x(), NewPos->y(), NewPos->z(), NewPos->v());
            
            
            break;
            }
            
         default:

            break;
        }
        

    }

    return nullptr;
}


void GameRole::Fini()
{
    //����Χ��ҷ���������Ϣ
    GameMsg* pmsg ;

    //��Χ�����
    std::list<Player*> srd_list = world.GetSrdPlayers(this);
    for (auto& single : srd_list)
    {
        pmsg = CreateIDNameLogoff();
        auto pRole = dynamic_cast<GameRole*>(single);
        ZinxKernel::Zinx_SendOut(*pmsg, *(pRole->m_pProto));

    }

    world.DelPlayer(this);



    //�ж��Ƿ������һ����� �ǵĻ� ������ʱ��
    if (ZinxKernel::Zinx_GetAllRole().size() == 0)
    {
        //�˳���ʱ��
        TimerOutMng::GetInstance().AddTask(&g_exit_timer);
    }


    //���ļ���ɾ����ǰ����

    //1 ���ļ��ж�ȡ��������
    //std::list<std::string> cur_name_list;
    //std::ifstream input_stream("/tmp/name_record");
    //std::string tmp;

    //while (getline(input_stream, tmp))
    //{
    //    cur_name_list.emplace_back(tmp);
    //}
    ////2 ɾ����ǰ����
    ////cur_name_list.remove(m_szName);
    ////3 д����������
    //std::ofstream output_stream("/tmp/name_record");
    //for (auto& name : cur_name_list)
    //{
    //    if (name != m_szName)
    //    {
    //        output_stream << name << std::endl;
    //    }
    //}

    //��redis game_nameɾ����ǰ����
    redisContext* context = redisConnect("127.0.0.1", 6379);

    if (nullptr != context)
    {
        redisReply* reply = (redisReply*)redisCommand(context, "lrem game_name 1 %s", m_szName.c_str());

        freeReplyObject(reply);
        redisFree(context);
    }

}

int GameRole::GetX()
{
    return (int)x;
}

int GameRole::GetY()
{
    return (int)z;
}
