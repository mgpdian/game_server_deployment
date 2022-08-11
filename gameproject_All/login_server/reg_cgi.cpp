#include <string>
#include <stdlib.h>
#include "user_opt.h"
#include <fcgi_stdio.h>


int main()
{
	while(FCGI_Accept() >= 0)
	{
		char buff[1024] = {0};
		fread(buff, 1, sizeof(buff), stdin);
		std::string orig_string(buff);
		
		int and_pos = orig_string.find("&");
		
		std::string username = orig_string.substr(9, and_pos - 9);
		std::string password = orig_string.substr(and_pos + 10, orig_string.size() - and_pos - 10);

		printf("Content-Type=text\r\n\r\n");
		if(true == CheckUser(username.c_str(), NULL))
		{
			//user exist
			printf("User Exist\r\n");
		}
		else{
			AddUser(username.c_str(), password.c_str());
			//register success
			printf("Success\r\n");
		}

	}

}
  