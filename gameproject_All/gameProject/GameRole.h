#pragma once
#include <zinx.h>
#include "AOIWorld.h"
#include "GameMsg.h"

class GameProtocol;

class GameRole :
    public Irole, public Player
{
private:
    GameProtocol* m_pProto = nullptr;

    float x = 0;
    float y = 0;//高
    float z = 0; //这个才是我们平时说的y
    float v = 0;//视角

    int m_iPid = 0;
    std::string m_szName;

    //封装 id 和 name 用于登录
    GameMsg* CreateIDNameLogin();

    //封装周围 玩家的位置
    GameMsg* CreateSrdPlayers();

    //封装自己的位置 给 周围的玩家
    GameMsg* CreateSelfPostion();

    //封装 id 和 name 用于下线
    GameMsg* CreateIDNameLogoff();

    //封装 玩家的聊天消息 发送给所有玩家
    GameMsg* CreateTalkBroadCast(std::string _content);


public:
    GameRole();
    virtual ~GameRole();

    void setProtocol(GameProtocol* _pProto);
    GameProtocol* getProtocol();

    //发送聊天信息
    void ProcTalkMsg(std::string _content);

    //向周围玩家发送位置
    void ProcMoveMsg(int _x, int _y, int _z, int _v);

    //当 进入新的玩家视野范围  视野出现
    void ViewAppear(GameRole* _pRole);

    //当 离开旧的玩家视野范围  视野消失
    void ViewLost(GameRole* _pRole);
public:
    // 通过 Irole 继承
    virtual bool Init() override;
    virtual UserData* ProcMsg(UserData& _poUserData) override;
    virtual void Fini() override;


    // 通过 Player 继承
    virtual int GetX() override;
    virtual int GetY() override;
};

