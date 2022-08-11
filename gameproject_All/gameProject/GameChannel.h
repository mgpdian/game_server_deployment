#pragma once
#include <zinx.h>
#include <ZinxTCP.h>
#include "GameProtocol.h"

class GameChannel :
    public ZinxTcpData
{
private:
    /// <summary>
    /// 由于是TCP连接 数据可能不会一次发送完成  
    /// 如果协议层只有一个 那多个通道连接  
    ///  而有几个客户端连接发生数据  协议层解析不可能去等待一个连接把数据发完 再去处理其他的连接
    /// 要解决这个问题的方法  就是每个通道都连接属于他自己的协议类 
    /// </summary>
    GameProtocol* m_proto = nullptr;
public:
    GameChannel(int _fd);
    virtual ~GameChannel();

    void setProtocol(GameProtocol* _proto);
    // 通过 ZinxTcpData 继承
    //返回协议对象
    virtual AZinxHandler* GetInputNextStage(BytesMsg& _oInput) override;
};


//GameChannel 的工厂类
class GameConnFact : public IZinxTcpConnFact
{
    // 通过 IZinxTcpConnFact 继承
    virtual ZinxTcpData* CreateTcpDataChannel(int _fd) override;
};
