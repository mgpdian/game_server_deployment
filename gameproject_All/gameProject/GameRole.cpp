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
//创建游戏世界全局单例对象
static AOIWorld world(0, 400, 0, 400, 20, 20);

//创建姓名池
 RandomName random_name;

//封装 id 和 name 用于登录   1
GameMsg* GameRole::CreateIDNameLogin()
{

    pb::SyncPid* pMsg = new pb::SyncPid();
    pMsg->set_pid(m_iPid);
    pMsg->set_username(m_szName);

    GameMsg* pRet = new GameMsg(GameMsg::MSG_TYPE_LOGIN_ID_NAME, pMsg);
    return pRet;
    
}

//封装周围 玩家的位置 202
GameMsg* GameRole::CreateSrdPlayers()
{

    pb::SyncPlayers* pMsg = new pb::SyncPlayers();
    
    //周围的玩家
    std::list<Player*> srd_list = world.GetSrdPlayers(this);

    for (auto single : srd_list)
    {
        auto pPlayer = pMsg->add_ps();
        auto pRole = dynamic_cast<GameRole*>(single);
        pPlayer->set_pid(pRole->m_iPid);
        pPlayer->set_username(pRole->m_szName);
        
        //位置设置
        auto pPostion = pPlayer->mutable_p();
        pPostion->set_x(pRole->x);
        pPostion->set_y(pRole->y);
        pPostion->set_z(pRole->z);
        pPostion->set_v(pRole->v);

    }

    
    GameMsg* pRet = new GameMsg(GameMsg::MSG_TYPE_SRD_POSITION, pMsg);
    return pRet;
}

//封装自己的位置 给 周围的玩家 200
GameMsg* GameRole::CreateSelfPostion()
{
    pb::BroadCast* pMsg = new pb::BroadCast();
    //这个为广播类 编号为200
    //玩家ID，聊天内容/初始位置/动作（预留）/新位置
    pMsg->set_pid(m_iPid);
    pMsg->set_username(m_szName);

    //设置位置消息 编号二 初始位置
    pMsg->set_tp(2);

    auto pPostition = pMsg->mutable_p();
    pPostition->set_x(x);
    pPostition->set_y(y);
    pPostition->set_z(z);
    pPostition->set_v(v);


    GameMsg* pRet = new GameMsg(GameMsg::MSG_TYPE_BROADCAST, pMsg);
    return pRet;
}
 
//封装 id 和 name  用于下线 201
GameMsg* GameRole::CreateIDNameLogoff()
{
    pb::SyncPid* pMsg = new pb::SyncPid();
    pMsg->set_pid(m_iPid);
    pMsg->set_username(m_szName);

    //和 上线的消息不同在与 他的消息ID为201
    GameMsg* pRet = new GameMsg(GameMsg::MSG_TYPE_LOGOFF_ID_NAME, pMsg);
    return pRet;
}


//封装玩家的聊天消息 发送给所有玩家 200
GameMsg* GameRole::CreateTalkBroadCast(std::string _content)
{
    pb::BroadCast* pMsg = new pb::BroadCast();
    //这个为广播类 编号为200
    //玩家ID，聊天内容/初始位置/动作（预留）/新位置
    pMsg->set_pid(m_iPid);
    pMsg->set_username(m_szName);

    //设置聊天消息 编号一 聊天内容
    pMsg->set_tp(1);

    pMsg->set_content(_content);

    GameMsg* pRet = new GameMsg(GameMsg::MSG_TYPE_BROADCAST, pMsg);

    return pRet;
}

static std::default_random_engine random_engine(time(NULL));

GameRole::GameRole()
{
    ////设置玩家ID为当前连接的fd
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
//发送聊天信息
void GameRole::ProcTalkMsg(std::string _content)
{
    //获取所有玩家
    auto role_list = ZinxKernel::Zinx_GetAllRole();

    GameMsg* pmsg;
    for (auto& pRole : role_list)
    {
        pmsg = CreateTalkBroadCast(_content);
        auto pGameRole = dynamic_cast<GameRole*>(pRole);
        ZinxKernel::Zinx_SendOut(*pmsg, *(pGameRole->m_pProto));
    }
}
//向周围玩家发送位置
void GameRole::ProcMoveMsg(int _x, int _y, int _z, int _v)
{
    //1 跨网格处理
    // 获取原来邻居s1
    std::list<Player*> srd_list_old = world.GetSrdPlayers(this);
    // 更新坐标 摘除旧网格, 添加新网格 得到新的邻居s2
    world.DelPlayer(this);
    x = _x;
    y = _y;
    z = _z;
    v = _v;
    world.AddPlayer(this);

    std::list<Player*> srd_list_new = world.GetSrdPlayers(this);
    // 遍历s2 若元素不属于s1 视野中出现
    for (auto& single_player : srd_list_new)
    {
        if (srd_list_old.end() == find(srd_list_old.begin(), srd_list_old.end(), single_player))
        {
            //视野中出现
            ViewAppear(dynamic_cast<GameRole*>(single_player));
        }
    }
    // 遍历s1 若元素不属于s2  视野中消失
    for (auto &single_player : srd_list_old)
    {
        if (srd_list_new.end() == find(srd_list_new.begin(), srd_list_new.end(), single_player))
        {
            //视野消失
            ViewLost(dynamic_cast<GameRole*>(single_player));
        }
    }
    //2广播新位置给周围玩家 
    
    std::list<Player*> srd_list = world.GetSrdPlayers(this);
    for (auto& single : srd_list)
    {
        //组成待发送的报文
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
//当 进入新的玩家视野范围  视野出现
void GameRole::ViewAppear(GameRole* _pRole)
{
    //向自己发送参数玩家的200消息
    GameMsg* pmsg = _pRole->CreateSelfPostion();
    ZinxKernel::Zinx_SendOut(*pmsg, *this->m_pProto);
    
    //向参数玩家发送自己的202消息
    pmsg = this->CreateSelfPostion();
    ZinxKernel::Zinx_SendOut(*pmsg, *(_pRole->m_pProto));
}
//当 离开旧的玩家视野范围  视野消失
void GameRole::ViewLost(GameRole* _pRole)
{
    //向自己发送参数玩家的201消息
    GameMsg* pmsg = _pRole->CreateIDNameLogoff();
    ZinxKernel::Zinx_SendOut(*pmsg, *this->m_pProto);

    //向参数玩家发送自己的201消息
    pmsg = this->CreateIDNameLogoff();
    ZinxKernel::Zinx_SendOut(*pmsg, *(_pRole->m_pProto));
}

//退出定时器的设置
class ExitTimer : public TimerOutProc {
    // 通过 TimerOutProc 继承
    virtual void Proc() override
    {
        ZinxKernel::Zinx_Exit();
    }
    virtual int GetTimeSec() override
    {
        return 20;
    }
};
//全局定时器
static ExitTimer g_exit_timer;


bool GameRole::Init()
{
    if (ZinxKernel::Zinx_GetAllRole().size() <= 0)
    {
        TimerOutMng::GetInstance().DelTask(&g_exit_timer);
    }


    //添加自己到游戏世界
    bool bRet = false;
    //设置玩家ID为当前连接的fd
    //为什么写在构造那里会报错  因为 构造的时候 没有绑定GameProtocol 当然会报错了
    m_iPid = this->m_pProto->getGameChannel()->GetFd();

    bRet = world.AddPlayer(this);

    
    
    if (true == bRet)
    {
        //向自己发生ID和名称
        GameMsg* pmsg = CreateIDNameLogin();
        //Zinx_SendOut的作用是 直接将数据发送给协议层
        ZinxKernel::Zinx_SendOut(*pmsg,*this->m_pProto);

        //向自己发送周围玩家的位置
        pmsg = CreateSrdPlayers();
        ZinxKernel::Zinx_SendOut(*pmsg, *this->m_pProto);

        //向周围玩家发送自己的位置
        
        //周围的玩家
        std::list<Player*> srd_list = world.GetSrdPlayers(this);
        for (auto& single : srd_list)
        {
            pmsg = CreateSelfPostion();
            auto pRole = dynamic_cast<GameRole*>(single);
            ZinxKernel::Zinx_SendOut(*pmsg, *(pRole->m_pProto));

        }
    }

    //以追加的形式 将 记录当前姓名到文件
   /* std::ofstream name_record("/tmp/name_record", std::ios::app);
    name_record << m_szName << std::endl;*/

    //记录当前名字到redis的game_name中
    //1 连接redis
    
    redisContext* context = redisConnect("127.0.0.1", 6379);

    //2 发送lpush命令
    if (nullptr != context)
    {
        redisReply* reply = (redisReply*)redisCommand(context, "lpush game_name %s", m_szName.c_str());

        //释放查询的内容的内存
        freeReplyObject(reply);

        //释放连接
        redisFree(context);
    }


    return bRet;
}

//处理游戏相关的用户请求
UserData* GameRole::ProcMsg(UserData& _poUserData)
{
    //测试: 打印消息内容
    GET_REF2DATA(MultiMsg, input, _poUserData);

    for (auto& single : input.m_Msgs)
    {
        std::cout << "type is " << single->getMsgType() << std::endl;
        std::cout  << single->getMsg()->Utf8DebugString() << std::endl;

        
        //根据类型选择处理
        switch (single->getMsgType())
        {
            //判断出 聊天内容 发送给所有玩家
        case GameMsg::MSG_TYPE_CHAT_CONTENT:{
            //取出聊天内容
            std::string content = dynamic_cast<pb::Talk*>(single->getMsg())->content();
            ProcTalkMsg(content);
           
            break;
            }
        case GameMsg::MSG_TYPE_NEW_POSITION :{
             // //1 跨网格处理
            //2广播新位置给周围玩家    
                
                
            //取出新位置
            pb::Position* NewPos = dynamic_cast<pb::Position*>(single->getMsg());
            //遍历周围玩家 发送
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
    //向周围玩家发送下线消息
    GameMsg* pmsg ;

    //周围的玩家
    std::list<Player*> srd_list = world.GetSrdPlayers(this);
    for (auto& single : srd_list)
    {
        pmsg = CreateIDNameLogoff();
        auto pRole = dynamic_cast<GameRole*>(single);
        ZinxKernel::Zinx_SendOut(*pmsg, *(pRole->m_pProto));

    }

    world.DelPlayer(this);



    //判断是否是最后一个玩家 是的话 启动定时器
    if (ZinxKernel::Zinx_GetAllRole().size() == 0)
    {
        //退出定时器
        TimerOutMng::GetInstance().AddTask(&g_exit_timer);
    }


    //从文件中删除当前姓名

    //1 从文件中读取所有姓名
    //std::list<std::string> cur_name_list;
    //std::ifstream input_stream("/tmp/name_record");
    //std::string tmp;

    //while (getline(input_stream, tmp))
    //{
    //    cur_name_list.emplace_back(tmp);
    //}
    ////2 删除当前姓名
    ////cur_name_list.remove(m_szName);
    ////3 写入其余姓名
    //std::ofstream output_stream("/tmp/name_record");
    //for (auto& name : cur_name_list)
    //{
    //    if (name != m_szName)
    //    {
    //        output_stream << name << std::endl;
    //    }
    //}

    //从redis game_name删除当前姓名
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
