# 48 thrift实现分布式容器创建

通过thrift 来代替 复杂的 redis发布和订阅  节约性能

thrift文件

room_opt.thrift

```c++
service container_mng{
        i32 create_room(1:i32 no)
}
```



thrift编译

```c++
thrift -r --gen cpp room_opt.thrift
```



补充生成的`container_mng_server.skeleton.cpp`

```c++
class container_mngHandler : virtual public container_mngIf {
 public:
  container_mngHandler() {
    // Your initialization goes here
  }

  int32_t create_room(const int32_t no) {
    // Your implementation goes here
        int32_t ret = 0;
        std::string cmd = "./create_room.sh room_no" + std::to_string(no);
        auto pf = popen(cmd.c_str(), "r");
        if(pf != NULL)
        {
                char buff[1024];
                fread(buff, 1, sizeof(buff), pf);
                ret = atoi(buff);
                pclose(pf);
        }
        return ret;
  }

};

```

编译

```c++
g++ -std=c++11 *.cpp -o container_mnger -lthrift -lssl -lcrypto

```







修改 `user_opt.cpp`

修改他 创建房间  使用thrift的客户端

```c++
std::string create_container(int _room_no)
{
	//return "x.x.x.x xxxxx"
	//1 确定调用在哪个服务器上创建
	//std::string port;
	std::string server_array[2] = {"192.168.81.128", "192.168.81.129"};
	std::string cur_server = server_array[g_cur_select++];
	g_cur_select %= sizeof(server_array)/sizeof(server_array[0]);

	std::shared_ptr<apache::thrift::transport::TTransport> socket(new apache::thrift::transport::TSocket(cur_server.c_str(), 9090));
	std::shared_ptr<apache::thrift::protocol::TProtocol> protocol(new apache::thrift::protocol::TBinaryProtocol(socket));

	container_mngClient client(protocol);

	socket->open();
	int port = client.create_room(_room_no);

	return cur_server + " " + std::to_string(port);
}

```

编译

```c++
g++ -std=c++11 login_cgi.cpp user_opt.cpp cJSON.c CJsonObject.cpp ./gen-cpp/container_mng.cpp -lfcgi -lhiredis -lthrift -lssl -lcrypto -o login_cgi
```

这样的化 就可以不在使用订阅和发布  还要一直设置那个MY_IP了 

现在从服务器需要启动`container-mng`就可以实现 创建房间和返回端口的功能 



部署 



对于login_cgi

```c++
g++ -std=c++11 login_cgi.cpp user_opt.cpp cJSON.c CJsonObject.cpp -lfcgi -lhiredis -o login_cgi
```

设置FCGI

```c++
spawn-fcgi -a 127.0.0.1 -p 8888 -f ./login_cgi
```



删除旧容器和旧镜像

```c++
docker rm -f `docker ps -aq`
```

```c++
docker rmi game:V1.0
```

重新设置生成新镜像

```c++
docker build -t game:V1.0 .
```



开始测试

启动从服务器监听器

```c++
./container_mnger
```

测试成功