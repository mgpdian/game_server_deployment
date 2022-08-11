# 10 Nginx 配置反向代理

反向代理就是  用户输入的url会先到反向代理那里 分析 然后分配到不同的服务器去  那实现负载均衡

在nginx.conf的server配置段添加：

```
location /1/{
    proxy_pass http://www.baidu.com/;
}
location /2/{
    proxy_pass http://www.sina.com/;
}
```

![反向代理示意图](10 Nginx 配置反向代理.assets/反向代理示意图-16564914045111.png)

proxy 翻译 代理

```bash
server{
        root /home/mgpdian;
        listen 81;
        location /{
              index  itcast.html;
            }
            location /mm/{
                    index http://www.baidu.com/;
            } //这里错误
            location /11/{
                    proxy_pass http://www.sina.com/;
            }
    }
```

这里的

```c++
 location /mm/{
                    index http://www.baidu.com/;
            }
```

是错误的  无法跳转404  因为index只会查看 当前目录的路径 不会进行那种服务器跳转的功能

## **nginx配置反向代理**

- 规划url和对应的网页服务器

- proxy_pass 被代理的服务;