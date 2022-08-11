#include <cstdio>
#include "calculate.h"
#include <iostream>
#include <memory>
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/transport/TSocket.h>
int main()
{
    //这是通信协议
    std::shared_ptr< apache::thrift::protocol::TTransport> socket(new apache::thrift::transport::TSocket("192.168.81.128", 9090));

    //这是二进制 它将通信协议写了进去
    std::shared_ptr<apache::thrift::protocol::TProtocol> protocol(new apache::thrift::protocol::TBinaryProtocol(socket));
    
    //连接服务器
    socket->open();

    //调用远程 需要传输给远程服务器 通信协议和 二进制化
    calculateClient client(protocol);

    std::cout << client.plus(1, 1) << std::endl;
    
    getchar();
    
    return 0;
}