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
    float y = 0;//��
    float z = 0; //�����������ƽʱ˵��y
    float v = 0;//�ӽ�

    int m_iPid = 0;
    std::string m_szName;

    //��װ id �� name ���ڵ�¼
    GameMsg* CreateIDNameLogin();

    //��װ��Χ ��ҵ�λ��
    GameMsg* CreateSrdPlayers();

    //��װ�Լ���λ�� �� ��Χ�����
    GameMsg* CreateSelfPostion();

    //��װ id �� name ��������
    GameMsg* CreateIDNameLogoff();

    //��װ ��ҵ�������Ϣ ���͸��������
    GameMsg* CreateTalkBroadCast(std::string _content);


public:
    GameRole();
    virtual ~GameRole();

    void setProtocol(GameProtocol* _pProto);
    GameProtocol* getProtocol();

    //����������Ϣ
    void ProcTalkMsg(std::string _content);

    //����Χ��ҷ���λ��
    void ProcMoveMsg(int _x, int _y, int _z, int _v);

    //�� �����µ������Ұ��Χ  ��Ұ����
    void ViewAppear(GameRole* _pRole);

    //�� �뿪�ɵ������Ұ��Χ  ��Ұ��ʧ
    void ViewLost(GameRole* _pRole);
public:
    // ͨ�� Irole �̳�
    virtual bool Init() override;
    virtual UserData* ProcMsg(UserData& _poUserData) override;
    virtual void Fini() override;


    // ͨ�� Player �̳�
    virtual int GetX() override;
    virtual int GetY() override;
};

