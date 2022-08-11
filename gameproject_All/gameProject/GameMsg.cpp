#include "GameMsg.h"
#include "msg.pb.h"
GameMsg::GameMsg()
{
}

GameMsg::~GameMsg()
{
	if (nullptr != m_pMsg)
	{
		delete m_pMsg;
	}
}

GameMsg::GameMsg(MSG_TYPE _type, google::protobuf::Message* _pMsg) : m_enMsgType(_type), m_pMsg(_pMsg)
{
}

GameMsg::GameMsg(MSG_TYPE _type, std::string _stream):m_enMsgType(_type)
{
	//通过简单工厂构造具体消息对象
	switch (_type)
	{
	case GameMsg::MSG_TYPE_LOGIN_ID_NAME: 
		m_pMsg = new pb::SyncPid();//玩家ID和玩家姓名
		break;
	case GameMsg::MSG_TYPE_CHAT_CONTENT: 
		m_pMsg = new pb::Talk();//聊天内容
		break;
	case GameMsg::MSG_TYPE_NEW_POSITION:
		m_pMsg = new pb::Position();//新位置
		break;
	case GameMsg::MSG_TYPE_BROADCAST:
		m_pMsg = new pb::BroadCast();//玩家ID，聊天内容/初始位置/动作（预留）/新位置
		break;
	case GameMsg::MSG_TYPE_LOGOFF_ID_NAME:
		m_pMsg = new pb::SyncPid();//玩家ID和玩家姓名
		break;
	case GameMsg::MSG_TYPE_SRD_POSITION:
		m_pMsg = new pb::SyncPlayers();//周围玩家们的位置
		break;
	default:
		break;
	}
	
	//将参数解析成消息对象内容
	m_pMsg->ParseFromString(_stream);
	
}

google::protobuf::Message* GameMsg::getMsg()
{
	return m_pMsg;
}


//序列化本消息
std::string GameMsg::serialize()
{
	std::string ret;
	m_pMsg->SerializeToString(&ret);
	return ret;
}

int GameMsg::getMsgType()
{
	return m_enMsgType;
}

MultiMsg::MultiMsg()
{
}

MultiMsg::~MultiMsg()
{
	for (auto& m_Msg : m_Msgs)
	{
		delete m_Msg;
	}
}
