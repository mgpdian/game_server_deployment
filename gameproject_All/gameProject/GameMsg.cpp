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
	//ͨ���򵥹������������Ϣ����
	switch (_type)
	{
	case GameMsg::MSG_TYPE_LOGIN_ID_NAME: 
		m_pMsg = new pb::SyncPid();//���ID���������
		break;
	case GameMsg::MSG_TYPE_CHAT_CONTENT: 
		m_pMsg = new pb::Talk();//��������
		break;
	case GameMsg::MSG_TYPE_NEW_POSITION:
		m_pMsg = new pb::Position();//��λ��
		break;
	case GameMsg::MSG_TYPE_BROADCAST:
		m_pMsg = new pb::BroadCast();//���ID����������/��ʼλ��/������Ԥ����/��λ��
		break;
	case GameMsg::MSG_TYPE_LOGOFF_ID_NAME:
		m_pMsg = new pb::SyncPid();//���ID���������
		break;
	case GameMsg::MSG_TYPE_SRD_POSITION:
		m_pMsg = new pb::SyncPlayers();//��Χ����ǵ�λ��
		break;
	default:
		break;
	}
	
	//��������������Ϣ��������
	m_pMsg->ParseFromString(_stream);
	
}

google::protobuf::Message* GameMsg::getMsg()
{
	return m_pMsg;
}


//���л�����Ϣ
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
