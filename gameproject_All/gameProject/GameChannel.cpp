#include "GameChannel.h"
#include "GameRole.h"
GameChannel::GameChannel(int _fd): ZinxTcpData(_fd)
{
}

GameChannel::~GameChannel()
{
    if (nullptr != m_proto)
    {
        ZinxKernel::Zinx_Del_Proto(*m_proto);
        delete m_proto;
    }
    //��ͨ���ฺ������ר��Э���������
}

void GameChannel::setProtocol(GameProtocol* _proto)
{
    this->m_proto = _proto; 
}

AZinxHandler* GameChannel::GetInputNextStage(BytesMsg& _oInput)
{
    return m_proto;
}

ZinxTcpData* GameConnFact::CreateTcpDataChannel(int _fd)
{
    //ͨ�������� ����һ��GameChannel();
    //����TCPͨ������
    GameChannel* pChannel = new GameChannel(_fd);
    //������Ӧ��Э�����
    GameProtocol* pProtocol = new GameProtocol();
    //������Ӧ��ҵ�����
    GameRole* pRole = new GameRole();
    //��Э�����
    pChannel->setProtocol(pProtocol);
    //Э����� �� ͨ������
    pProtocol->setGameChannel(pChannel);

    //Э����� �� ҵ�����
    pProtocol->setGameRole(pRole);
    // ҵ����� Ҳ�� Э�����
    pRole->setProtocol(pProtocol);
    //��Э�������ӵ�Kernel��

    /*���Э�����Ҫ�����Ϊ�Ѷ��󣬸ú��������ڴ��������ã����鿪���߽�Э�������ӵ�zinxkernel�洢*/
    ZinxKernel::Zinx_Add_Proto(*pProtocol);

    //��zinxkernel �� ��� ҵ��� ����
    ZinxKernel::Zinx_Add_Role(*pRole);

    //Ϊʲô��Ҫ���ͨ������� ��ΪZinxTcpListen������� ����Ŀ���

    return pChannel;
}
