# 8 业务类 结合AOI 玩家上下线功能

首先是需求的前三步 来实现玩家上线时需要的功能

- 新客户端连接后，向其发送ID和名称
- 新客户端连接后，向其发送**周围**玩家的位置
- 新客户端连接后，向**周围**玩家发送其位置

最后添加一步 实现玩家下线的功能

- 客户端断开时，向**周围**玩家发送其断开的消息

AOI结合GameRole类

- 继承player类 重写getx和gety ---> 需要返回的是z坐标 因为实际y是高
- 创建唯一游戏世界对象(全局对象)
- gamerole初始化(init函数)时添加自己到游戏世界中
- 去初始化时 摘除自己

# 玩家上线需要实现的功能

## 第一步 实现 发送自己的ID和名字

新客户端连接后，向其发送ID和名称

### 实际实现 

GameRole 多重继承Player类 并且添加长宽高以及视角

```c++

class GameProtocol;
class GameMsg;
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

    GameMsg* CreateIDNameLogin();
public:
    GameRole();
    virtual ~GameRole();

    void setProtocol(GameProtocol* _pProto);
    GameProtocol* getProtocol();
public:
    // 通过 Irole 继承
    virtual bool Init() override;
    virtual UserData* ProcMsg(UserData& _poUserData) override;
    virtual void Fini() override;


    // 通过 Player 继承
    virtual int GetX() override;
    virtual int GetY() override;
};

```

### 具体实现

创建游戏地图的全局单例

```c++

//创建游戏世界全局单例对象
static AOIWorld world(0, 400, 0, 400, 20, 20);
```



将GameRole (现在的GameRole继承着player 他自己就是player)

添加到 世界地图中(自动分配到网格里)

```c++
//这样做 可以检测到异常
bool GameRole::Init()
{
    //添加自己到游戏世界
    bool bRet = false;
    bRet = world.AddPlayer(this);
    return bRet;
}
```

同样在用户注销时 会触发Fini 去初始化 退出网格

```c++
void GameRole::Fini()
{
    world.DelPlayer(this);
}
```

返回用户坐标

```c++

int GameRole::GetX()
{
    return (int)x;
}
//注意返回的是z
int GameRole::GetY()
{
    return (int)z;
}

```





实现登录时 返回玩家位置 (因为玩家位置是随机的 要让客户端知道在哪)

类中添加  m_iPid记录玩家连接使用的Fd    

CreateIDNameLogin() 是生成一个返回给协议类的一个Gamemsg 携带着id和name

```c++
 int m_iPid = 0;
    std::string m_szName;

    GameMsg* CreateIDNameLogin();
```

```c++

GameMsg* GameRole::CreateIDNameLogin()
{

    pb::SyncPid* pmsg = new pb::SyncPid();
    pmsg->set_pid(m_iPid);
    pmsg->set_username(m_szName);

    GameMsg* pRet = new GameMsg(GameMsg::MSG_TYPE_LOGIN_ID_NAME, pmsg);
    return pRet;
    
}
```



## 难点

#### GameRole的m_iPid的绑定

一开始使用构造函数去 创建id

```c++
GameRole::GameRole()
{
    ////设置玩家ID为当前连接的fd
    //m_iPid = m_pProto->getGameChannel()->GetFd();
    this->m_szName = "Tom";
}
```

> m_iPid = m_pProto->getGameChannel()->GetFd();

然后就报错 

原因是: 生成时 并没有绑定协议类 这时候去找fd 当然会报错

解决方法 等到初始化时 (即玩家加入网格时)再去绑定fd

```c++


bool GameRole::Init()
{
    //添加自己到游戏世界
    bool bRet = false;
    //设置玩家ID为当前连接的fd
    //为什么写在构造那里会报错  因为 构造的时候 没有绑定GameProtocol 当然会报错了
    m_iPid = this->m_pProto->getGameChannel()->GetFd();

    bRet = world.AddPlayer(this);

    
    //向自己发生ID和名称
    if (true == bRet)
    {
        auto pmsg = CreateIDNameLogin();
        
        ZinxKernel::Zinx_SendOut(*pmsg,*this->m_pProto);
    }

    return bRet;
}
```

## 第二步 实现发送周围的玩家位置

- 新客户端连接后，向其发送**周围**玩家的位置
- 

和之前一样  使用一个函数来封装周围玩家位置

```c++
//封装周围 玩家的位置
    GameMsg* CreateSrdPlayers();
```

```c++

//封装周围 玩家的位置
GameMsg* GameRole::CreateSrdPlayers()
{

    pb::SyncPlayers* pMsg = new pb::SyncPlayers();
    
    //周围的玩家
    std::list<Player*> srd_list = world.GetSrdPlayers(this);

    for (auto& single : srd_list)
    {
        auto pPlayer = pMsg->add_ps();
        auto pRole = dynamic_cast<GameRole*>(single);
        pPlayer->set_pid(pRole->m_iPid);
        pPlayer->set_username(pRole->m_szName);
        
        //位置设置
        auto pPostion = pPlayer->mutable_p();
        pPostion->set_x(pRole->x);
        pPostion->set_y(pRole->y);
        pPostion->set_z(pRole->z);
        pPostion->set_v(pRole->v);

    }
    GameMsg* pRet = new GameMsg(GameMsg::MSG_TYPE_SRD_POSITION, pMsg);
    return pRet;
}

```

有关protocol的东西 之后再学



发送

```c++

bool GameRole::Init()
{
    //添加自己到游戏世界
    bool bRet = false;
    //设置玩家ID为当前连接的fd
    //为什么写在构造那里会报错  因为 构造的时候 没有绑定GameProtocol 当然会报错了
    m_iPid = this->m_pProto->getGameChannel()->GetFd();

    bRet = world.AddPlayer(this);

    
    
    if (true == bRet)
    {
        //向自己发生ID和名称
        GameMsg* pmsg = CreateIDNameLogin();
        //Zinx_SendOut的作用是 直接将数据发送给协议层
        ZinxKernel::Zinx_SendOut(*pmsg,*this->m_pProto);

        //向自己发生周围玩家的位置
        pmsg = CreateSrdPlayers();
        ZinxKernel::Zinx_SendOut(*pmsg, *this->m_pProto);
    }

    return bRet;
}

```

## 第三步 玩家上线向周围玩家发送位置

新客户端连接后，向**周围**玩家发送其位置

和之前一样  使用一个函数自己的位置 给 周围的玩家

```c++
//封装自己的位置 给 周围的玩家
    GameMsg* CreateSelfPostion();
```

```c++

//封装自己的位置 给 周围的玩家
GameMsg* GameRole::CreateSelfPostion()
{
    pb::BroadCast* pMsg = new pb::BroadCast();
    //这个为广播类 编号为200
    //玩家ID，聊天内容/初始位置/动作（预留）/新位置
    pMsg->set_pid(m_iPid);
    pMsg->set_username(m_szName);

    //设置位置消息 编号二 初始位置
    pMsg->set_tp(2);

    auto pPostition = pMsg->mutable_p();
    pPostition->set_x(x);
    pPostition->set_y(y);
    pPostition->set_z(z);
    pPostition->set_v(v);


    GameMsg* pRet = new GameMsg(GameMsg::MSG_TYPE_BROADCAST, pMsg);
    return pRet;
}

```

```c++

bool GameRole::Init()
{
    //添加自己到游戏世界
    bool bRet = false;
    //设置玩家ID为当前连接的fd
    //为什么写在构造那里会报错  因为 构造的时候 没有绑定GameProtocol 当然会报错了
    m_iPid = this->m_pProto->getGameChannel()->GetFd();

    bRet = world.AddPlayer(this);

    
    
    if (true == bRet)
    {
        //向自己发生ID和名称
        GameMsg* pmsg = CreateIDNameLogin();
        //Zinx_SendOut的作用是 直接将数据发送给协议层
        ZinxKernel::Zinx_SendOut(*pmsg,*this->m_pProto);

        //向自己发生周围玩家的位置
        pmsg = CreateSrdPlayers();
        ZinxKernel::Zinx_SendOut(*pmsg, *this->m_pProto);

        //向周围玩家发送自己的位置
        
        //周围的玩家
        std::list<Player*> srd_list = world.GetSrdPlayers(this);
        for (auto& single : srd_list)
        {
            pmsg = CreateSelfPostion();
            auto pRole = dynamic_cast<GameRole*>(single);
            ZinxKernel::Zinx_SendOut(*pmsg, *(pRole->m_pProto));

        }

    }
    return bRet;
}

```

值得注意的是

```c++
for (auto& single : srd_list)
{
            pmsg = CreateSelfPostion();
}
```

这里的pmsg不能在循环外直接赋予 防止会导致 发送超过一个玩家的情况下 传送空的数据 导致崩溃

原因应该是 发送完一次消息后 后析构掉这个消息  所以我们要再次赋予pmsg数据



# 总结

#### 设置protobuf类型消息的repeated类型 

- add_XXXX函数
- 调用后，会向当前消息添加一个数组成员，返回数组成员的指针

#### 设置protobuf中复合类型 

- mutble_xxxx函数
- 调用后，会向当前消息添加子消息。返回子消息的指针



比如

```c++
pb::SyncPlayers* pMsg = new pb::SyncPlayers();
    
    //周围的玩家
    std::list<Player*> srd_list = world.GetSrdPlayers(this);

    for (auto& single : srd_list)
    {
        auto pPlayer = pMsg->add_ps();
        auto pRole = dynamic_cast<GameRole*>(single);
        pPlayer->set_pid(pRole->m_iPid);
        pPlayer->set_username(pRole->m_szName);
        
        //位置设置
        auto pPostion = pPlayer->mutable_p();
        pPostion->set_x(pRole->x);
        pPostion->set_y(pRole->y);
        pPostion->set_z(pRole->z);
        pPostion->set_v(pRole->v);

    }
```

add_xx 的情况 是要添加多个相同的类型



而 mutable_p 是 来设置复合类型时使用的







#### 连接到来（玩家初始化）时

- 属性pid赋值为socket值
- 属性name写成tom
- 初始坐标100,100
- 向自己发内容是ID和姓名的1号消息
- 向自己发内容是若干周围玩家信息的202号消息
- 向周围玩家发送内容是自己位置的200号消息









# 玩家下线实现的功能

最后添加一步 实现玩家下线的功能

- 客户端断开时，向**周围**玩家发送其断开的消息

## 向周围玩家发送自己断开的消息

和之前一样  使用一个函数自己的ID和姓名 发送给其他玩家  实现和第一个函数基本一样 

唯一的不同是 他使用的是201编号 代表他要从其他玩家 那里离开

```c++
//封装 id 和 name 用于下线
    GameMsg* CreateIDNameLogoff();

//封装 id 和 name  用于下线
GameMsg* GameRole::CreateIDNameLogoff()
{
    pb::SyncPid* pMsg = new pb::SyncPid();
    pMsg->set_pid(m_iPid);
    pMsg->set_username(m_szName);

    //和 上线的消息不同在与 他的消息ID为201
    GameMsg* pRet = new GameMsg(GameMsg::MSG_TYPE_LOGOFF_ID_NAME, pMsg);
    return pRet;
}

```

具体实现 

在Fini  退出前 对其他玩家发送这个数据 

```c++

void GameRole::Fini()
{
    //向周围玩家发送下线消息
    GameMsg* pmsg ;

    //周围的玩家
    std::list<Player*> srd_list = world.GetSrdPlayers(this);
    for (auto& single : srd_list)
    {
        pmsg = CreateIDNameLogoff();
        auto pRole = dynamic_cast<GameRole*>(single);
        ZinxKernel::Zinx_SendOut(*pmsg, *(pRole->m_pProto));

    }

    world.DelPlayer(this);
}
```

