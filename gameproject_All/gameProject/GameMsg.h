#pragma once
#include <zinx.h>
#include <google/protobuf/message.h>
#include <list>
#include "msg.pb.h"

class GameMsg :
    public UserData
{

public:
    enum MSG_TYPE {
        MSG_TYPE_LOGIN_ID_NAME = 1,
        MSG_TYPE_CHAT_CONTENT = 2,
        MSG_TYPE_NEW_POSITION = 3,
        MSG_TYPE_BROADCAST = 200, //广播
        MSG_TYPE_LOGOFF_ID_NAME = 201,
        MSG_TYPE_SRD_POSITION = 202
    };
private:
    enum MSG_TYPE m_enMsgType;
    //客户端的请求的消息
    google::protobuf::Message * m_pMsg = nullptr;
public:
    GameMsg();
    virtual ~GameMsg();

    //已知消息内容创建消息对象
    GameMsg(MSG_TYPE _type, google::protobuf::Message* _pMsg);

    //将字节流内容转换成消息结构
    GameMsg(MSG_TYPE _type, std::string _stream);

    //获取google::protobuf::Message
    google::protobuf::Message* getMsg();

    //序列化本消息
    std::string serialize();


    //获取信号ID
    int getMsgType();

};

class MultiMsg : public UserData
{
public:

    std::list<GameMsg*> m_Msgs;

    MultiMsg();
    virtual ~MultiMsg();
};