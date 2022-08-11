#pragma once
#include <zinx.h>
#include <ZinxTCP.h>
#include "GameProtocol.h"

class GameChannel :
    public ZinxTcpData
{
private:
    /// <summary>
    /// ������TCP���� ���ݿ��ܲ���һ�η������  
    /// ���Э���ֻ��һ�� �Ƕ��ͨ������  
    ///  ���м����ͻ������ӷ�������  Э������������ȥ�ȴ�һ�����Ӱ����ݷ��� ��ȥ��������������
    /// Ҫ����������ķ���  ����ÿ��ͨ���������������Լ���Э���� 
    /// </summary>
    GameProtocol* m_proto = nullptr;
public:
    GameChannel(int _fd);
    virtual ~GameChannel();

    void setProtocol(GameProtocol* _proto);
    // ͨ�� ZinxTcpData �̳�
    //����Э�����
    virtual AZinxHandler* GetInputNextStage(BytesMsg& _oInput) override;
};


//GameChannel �Ĺ�����
class GameConnFact : public IZinxTcpConnFact
{
    // ͨ�� IZinxTcpConnFact �̳�
    virtual ZinxTcpData* CreateTcpDataChannel(int _fd) override;
};
