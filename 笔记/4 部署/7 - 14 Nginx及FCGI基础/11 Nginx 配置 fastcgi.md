# 11 Nginx 配置 fastcgi

nignx无法处理 网页发送过来的动态数据  所以需要FastCgi来处理

Nignx配置FastCGI 

- 目的: 跟客户端数据交互



fastcgi是用来处理动态请求的。当用户请求匹配到配置fastcgi的location时，请求会发给fastcgi程序处理。

添加如下配置段，意味着用户的每条请求都会封装成fastcgi标准的格式转发给127.0.0.1这台主机（本机）的12345端口。

```
location /fcgi/{
    fastcgi_pass 127.0.0.1:12345;
}
```

服务器可以通过编写丰富的fastcgi程序和用户进行多种互动。

接下来我们看一下怎样使用fastcgi。