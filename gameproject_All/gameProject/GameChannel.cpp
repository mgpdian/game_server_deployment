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
    //由通道类负责他的专属协议类的析构
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
    //通过工厂类 生成一个GameChannel();
    //创建TCP通道对象
    GameChannel* pChannel = new GameChannel(_fd);
    //创建对应的协议对象
    GameProtocol* pProtocol = new GameProtocol();
    //创建对应的业务对象
    GameRole* pRole = new GameRole();
    //绑定协议对象
    pChannel->setProtocol(pProtocol);
    //协议对象 绑定 通道对象
    pProtocol->setGameChannel(pChannel);

    //协议对象 绑定 业务对象
    pProtocol->setGameRole(pRole);
    // 业务对象 也绑定 协议对象
    pRole->setProtocol(pProtocol);
    //将协议对象添加到Kernel中

    /*添加协议对象，要求对象为堆对象，该函数仅有内存管理的作用，建议开发者将协议对象添加到zinxkernel存储*/
    ZinxKernel::Zinx_Add_Proto(*pProtocol);

    //在zinxkernel 中 添加 业务层 对象
    ZinxKernel::Zinx_Add_Role(*pRole);

    //为什么不要添加通道层对象 因为ZinxTcpListen帮你绑定了 我真的哭死

    return pChannel;
}
