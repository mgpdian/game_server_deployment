#include <cstdio>
#include <stdlib.h>
#include <iostream>
#include <hiredis/hiredis.h>
#include <hiredis/adapters/libevent.h>
#include <hiredis/async.h>
#include <event.h>
#include <string>


void empty(struct redisAsyncContext* pc, void* reply, void* private_data)
{
}
void my_callbask(struct redisAsyncContext* pc, void* reply, void* private_data)
{
    std::string port;
    std::string room_no;
    //若指定的服务器是自己才会创建房间
    redisReply* my_reply = (redisReply*)reply;
    
    if ("message" != std::string(my_reply->element[0]->str))
    {
        //刚连接
        
        return;
    }
    else {
        //reply 中 包括 IP: 房间号
        std::string orig_content(my_reply->element[2]->str);
        int pos = orig_content.find(':');
        std::string IP = orig_content.substr(0, pos);
        //判断IP是否是自己的IP
        //选择环境变量来获取IP  因为如果系统调用可能会获取到外网或者内网等各种各样的IP
        std::string my_ip(getenv("MY_IP"));
        if (IP != my_ip)
        {
            
            return;
        }
        room_no = orig_content.substr(pos + 1);
    }
   
    //执行脚本获取端口号
    std::string cmd = "./create_room.sh " + room_no;
    FILE* pf = popen(cmd.c_str(), "r");
    if (nullptr != pf)
    {
        char buff[1024];
        fread(buff, 1, sizeof(buff), pf);

        port.append(buff);
        port.pop_back();
        pclose(pf);
    }

    //发布端口号

    redisContext* spc = redisConnect("192.168.81.128", 6379);
    if (nullptr != spc)
    {
        freeReplyObject(redisCommand(spc, "publish server_port %s", port.c_str()));
        redisFree(spc);
    }
    //redisAsyncCommand(pc, empty, NULL, "publish server_port %s", port.c_str());

}


int main()
{
    auto base = event_base_new();

    redisAsyncContext* pc =  redisAsyncConnect("192.168.81.128", 6379);
    redisLibeventAttach(pc, base);

    redisAsyncCommand(pc, my_callbask, NULL, "subscribe create_room");

    event_base_dispatch(base);
    return 0;
}