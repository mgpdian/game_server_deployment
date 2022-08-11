# 16 存储当局玩家及redis实现

### 需求：查看当前局游戏内有哪些玩家？

1. 创建文件（/tmp（存到内存的，重启会消失））存储当前游戏局的
玩家们的名字
2. 查询：显示文件内容
3. 设置：存姓名到文件或从文件中取姓名
1. 存：追加的方式写文件
2. 删：读出所有内容，将非自己的名字重写写入

文件tmp为缓存目录 每次电脑重启 后会清空

同时作为缓存目录 他的运行是在内存中的(即 类似于Redis) 这样读取的速度更快



当玩家登录时  在Init广播自己的状态后  添加玩家名字进文件中 使用的是ios::app追加到文件后面

```c++

bool GameRole::Init()
{
    ....
    //以追加的形式 将 记录当前姓名到文件
    std::ofstream name_record("/tmp/name_record", std::ios::app);
    name_record << m_szName << std::endl;
    
    return bRet;
}
```

玩家退出时 删除

```c++
//从文件中删除当前姓名

    //1 从文件中读取所有姓名
    std::list<std::string> cur_name_list;
    std::ifstream input_stream("/tmp/name_record");
    std::string tmp;

    while (getline(input_stream, tmp))
    {
        cur_name_list.emplace_back(tmp);
    }
    //2 删除当前姓名
    //cur_name_list.remove(m_szName);
    //3 写入其余姓名
    std::ofstream output_stream("/tmp/name_record");
    for (auto& name : cur_name_list)
    {
        if (name != m_szName)
        {
            output_stream << name << std::endl;
        }
    }
```

很明显 这样做效率很低 

接下来会使用redis 来实现这个功能





# Redis

他的安装使用看[17章](G:\笔记\游戏服务器\笔记\2 游戏业务实现\17 Redis和hiredis.md)

### 添加功能

将玩家添加到redis的 game_name

```c++
//记录当前名字到redis的game_name中
    //1 连接redis
    
    redisContext* context = redisConnect("127.0.0.1", 6379);

    //2 发送lpush命令
    if (nullptr != context)
    {
        redisReply* reply = (redisReply*)redisCommand(context, "lpush game_name %s", m_szName.c_str());

        //释放查询的内容的内存
        freeReplyObject(reply);

        //释放连接
        redisFree(context);
    }
```



将玩家从redis的game_name中删除

```c++
//从redis game_name删除当前姓名
    redisContext* context = redisConnect("127.0.0.1", 6379);

    if (nullptr != context)
    {
        redisReply* reply = (redisReply*)redisCommand(context, "lrem game_name 1 %s", m_szName.c_str());

        freeReplyObject(reply);
        redisFree(context);
    }
```

