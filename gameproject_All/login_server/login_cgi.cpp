#include <stdlib.h>
#include <string>
#include "user_opt.h"
#include "CJsonObject.hpp"
#include <fcgi_stdio.h>
#include <hiredis/hiredis.h>

static int cur_room_no = 0;
int main()
{
	while (FCGI_Accept() >= 0)
	{
		std::string username;
		std::string password;

		int json_len = atoi(getenv("CONTENT_LENGTH")); //读取http传输的环境变量

		char *buff = (char *)calloc(1UL, json_len); // 1UL 设置全0

		fread(buff, 1, json_len, stdin);

		std::string json_string(buff);
		neb::CJsonObject json(json_string);

		json.Get("username", username);
		json.Get("password", password);

		printf("content-Type:application/json\r\n");

		neb::CJsonObject reply;
		if (true == CheckUser(username.c_str(), password.c_str()))
		{
			// reply ok to client
			reply.Add("login_result", "OK");
			//判断传输过来的opt状态 来选择是否建房
			std::string opt;
			json.Get("opt", opt);
			if("create_room" == opt)
			{
				cur_room_no++;
				std::string room_info = create_container(cur_room_no);
				//add a 数据 : room_noX room_info
				
				redisContext* pc = redisConnect("127.0.0.1", 6379);
				if(nullptr != pc)
				{
					
					freeReplyObject(redisCommand(pc, "set room_no%d %s", cur_room_no, room_info.c_str()));

					redisFree(pc);
				}
				
				reply.Add("server_IP", room_info.substr(0, room_info.find(' ')));
				reply.Add("server_PORT", room_info.substr(room_info.find(' ') + 1));
				reply.Add("room_no", cur_room_no);
			}
			else if("follow_room" == opt)
			{
				std::string room_no;
				json.Get("room_no", room_no);
				//reply.Add("ss", "room_no");
				std::string room_info = check_room(room_no.c_str());
				if(room_info.size() <= 0)
				{
					reply.Add("follow_result", "Failed");
				}
				else{
					reply.Add("follow_result", "OK");
					reply.Add("server_IP", room_info.substr(0, room_info.find(' ')));
					reply.Add("server_PORT", room_info.substr(room_info.find(' ') + 1));
				}
			}
		}
		else
		{
			// replay failed to client
			reply.Add("login_result", "Failed");
		}
		std::string reply_string = reply.ToFormattedString();
		printf("Content-Length:%d\r\n\r\n", reply_string.size());

		printf("%s\r\n", reply_string.c_str());

		free(buff);
	}
}
