#include "GameProtocol.h"
#include <iostream>
#include "GameMsg.h"
#include "GameChannel.h"
#include "GameRole.h"

//�������ͷ�ļ�

GameProtocol::GameProtocol()
{
}

GameProtocol::~GameProtocol()
{//������
    if (nullptr != m_role)
    {
        ZinxKernel::Zinx_Del_Role(*m_role);
        delete m_role;
    }
    //��Э���� ���� ר�Ŵ�������ҵ����
    
}

//���������ͨ��������ԭʼ����
//����ֵ��ת�������Ϣ����MultiMsg
//ת����ʽ TCPմ������
//|��Ϣ���ݵĳ��ȣ�4���ֽڣ����ֽ���ǰ��|��ϢID��4���ֽڣ����ֽ���ǰ��|��Ϣ����|
UserData* GameProtocol::raw2request(std::string _szInput)
{
    MultiMsg* pRet = new MultiMsg();
    //ƴ�ӵ�֮ǰ�ı�����
    m_dataCache.append(_szInput);


    while (true)
    {
        if (m_dataCache.size() < 8)
        {
            break;
        }
        //�������ֽ���ת��Ϊ�����ֽ���

        //��ǰ�ĸ��ֽ��ж�ȡ��Ϣ���ݳ���
        int iLength = 0;
        iLength |= m_dataCache[0] << 0;
        iLength |= m_dataCache[1] << 8;
        iLength |= m_dataCache[2] << 16;
        iLength |= m_dataCache[3] << 24;

        //���ĸ��ֽڶ�����id
        int id = 0;
        id |= m_dataCache[4] << 0;
        id |= m_dataCache[5] << 8;
        id |= m_dataCache[6] << 16;
        id |= m_dataCache[7] << 24;

        //ͨ�������ĳ����жϺ������Ƿ�Ϸ�
        if (m_dataCache.size() - 8 < iLength)
        {
            //�������ĳ��Ȳ��� ֱ�ӷ��� �ȴ���һ������  
            break;
        }

        //����һ���û�����
        GameMsg* pMsg = new GameMsg((GameMsg::MSG_TYPE)id, m_dataCache.substr(8, iLength));

        pRet->m_Msgs.emplace_back(pMsg);
        //�õ���ȡ��ʣ�µı���
        //m_dataCache = m_dataCache.substr(8 + iLength, m_dataCache.size() - 8 - iLength);
        //�Ƚ�Ч�ʵ�д��
        m_dataCache.erase(0, 8 + iLength);

    }
    
    //���� ��
    /*<----------------------------------------->
        recv from 5:09 00 00 00 01 00 00 00 08 01 12 05 6C 6C 6C 6C 6B
        < ----------------------------------------->
        Pid: 1
        Username : "llllk"*/
  /*  for (auto& single : pRet->m_Msgs)
    {
        std::cout << single->getMsg()->Utf8DebugString() << std::endl;
    }*/

    //���� ��
    /*pb::Talk* pmsg = new pb::Talk();
    pmsg->set_content("hello");
    GameMsg* pGameMsg = new GameMsg(GameMsg::MSG_TYPE_CHAT_CONTENT, pmsg);
    ZinxKernel::Zinx_SendOut(*(pGameMsg), *this);*/
    //���ص����� Ҳ���� ���� id  hello
    //0:04 : 18 �յ����ݣ�{ 07 00 00 00 02 00 00 00 0A 05 68 65 6C 6C 6F }
    return pRet;
}


//��������ҵ��� �����͵���Ϣ
//����ֵΪת������ֽ���
std::string* GameProtocol::response2raw(UserData& _oUserData)
{
    int iLength = 0;
    int id = 0;
    std::string MsgContent;

    GET_REF2DATA(GameMsg, oOutput, _oUserData);
    id = oOutput.getMsgType();
    MsgContent = oOutput.serialize(); //ʹ�����л������ַ���
    iLength = MsgContent.size();


    auto pret = new std::string();
    //Ҫ�����Ⱥ�id ת���������ֽ���
    //Ҫ��֮ǰ�ĵ�����
   
    pret->push_back((iLength >> 0) & 0xff);//ֻҪ��8λ ����Ҫ��0xff
    pret->push_back((iLength >> 8) & 0xff);
    pret->push_back((iLength >> 16) & 0xff);
    pret->push_back((iLength >> 24) & 0xff);


    pret->push_back((id >> 0) & 0xff); // ֻҪ��8λ ����Ҫ��0xff
    pret->push_back((id >> 8) & 0xff);
    pret->push_back((id >> 16) & 0xff);
    pret->push_back((id >> 24) & 0xff);


    pret->append(MsgContent); 
   
    return pret;
}

Irole* GameProtocol::GetMsgProcessor(UserDataMsg& _oUserDataMsg)
{
    return m_role;
}

//�������ݷ��͵�ͨ��
Ichannel* GameProtocol::GetMsgSender(BytesMsg& _oBytes)
{
    return m_channel;
}

void GameProtocol::setGameChannel(GameChannel* _pChannel)
{
    this->m_channel = _pChannel;
}

void GameProtocol::setGameRole(GameRole* _pRole)
{
    this->m_role = _pRole;
}

GameRole* GameProtocol::getGameRole()
{
    return this->m_role;
}

GameChannel* GameProtocol::getGameChannel()
{
    return this->m_channel;
}
