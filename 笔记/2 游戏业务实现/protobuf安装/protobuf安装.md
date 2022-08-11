# protobuf安装

**Ubuntu**

```text
// 下载 protoBuf：
$ git clone https://github.com/protocolbuffers/protobuf.git
//  安装依赖库
$ sudo apt-get install autoconf  automake  libtool curl make  g++  unzip libffi-dev -y
// 进入目录
$ cd protobuf/ 
// 自动生成configure配置文件：
$ ./autogen.sh 
// 配置环境：
$ ./configure
// 编译源代码(要有耐心！)：
$ make 
// 安装
$ sudo make install
// 刷新共享库 （很重要的一步啊）
$ sudo ldconfig 
// 成功后需要使用命令测试
$ protoc -h 
```

使用  

通过protoc -h  查看调用方法

得到: Usage: protoc [OPTION] PROTO_FILES

c++的调用为: --cpp_out=OUT_DIR

将msg.proto文件放入得

> protoc --cpp_out=. msg.proto

得到 mgs.pb.cc 和 mgs.pb.h  将他们移动项目文件夹中

![image-20220621201407621](protobuf安装.assets/image-20220621201407621.png)



