# 18 makefile编写

为了服务器移植的便捷性 

使用makefile来安装编码

首先把 放在bin/x64/Debug/ramdom_*  的文件 即俩个姓名文件取出到主文件目录那里

```c++
cp bin/x64/Debug/ramdom_* .
```

然后删除不需要的bin/ obj/

```c++
rm -rf bin/ obj/
```



接下来编写 makefile文件

```c++
game:*.cpp *h
    g++ -Wall -g -pthread -std=c++11 $^ -o $@ -lzinx -lpthread -lprotobuf
```





若要打包

删除生成的game运行文件

```c++
tar zcf game.tar.gz game/
```

