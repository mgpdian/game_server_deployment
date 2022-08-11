# 45 rpc概念

rpc为 remote process call (A主机调用B主机的函数(功能))

![image-20220703212107377](44 rpc概念.assets/image-20220703212107377.png)

### **RPC概念**

- 远程过程调用（A主机调用B主机的功能）
- A主机调用函数的过程中跟B主机进行若干通信得到运算结果
- B主机接收A主机的请求并进行计算
- 异构，跨语言，跨平台
- thrift，grpc 