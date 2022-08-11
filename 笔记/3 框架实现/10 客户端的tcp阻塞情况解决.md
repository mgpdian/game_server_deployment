# 10 客户端的tcp阻塞情况解决

因为可能连接服务器时  服务器忙碌没有里你

所以使用非阻塞来解决这个问题

```c++
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <malloc.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <stdarg.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/select.h>

int main()
{
    //创建socket
    int lfd = socket(AF_INET, SOCK_STREAM, 0);

    int flag = fcntl(lfd, F_GETFL);
    flag |= O_NONBLOCK;
    fcntl(lfd, F_SETFL, flag);

    //绑定
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(51111);
    inet_pton(AF_INET, "10.3.113.175", &addr.sin_addr.s_addr);

    //连接服务器
    //设置非阻塞后 connect若是没连上 会到 select那里连
    int ret = connect(lfd, (struct sockaddr*)&addr, sizeof(addr));


    // flag = fcntl(lfd, F_GETFL);
    // flag &= ~O_NONBLOCK;
    // fcntl(lfd, F_SETFL, flag);

    fd_set set;
    FD_ZERO(&set);
    FD_SET(lfd, &set);
    struct timeval to = {
        5, 0
    };

    int maxfd = lfd;
    int sel_ret = -1;
    if (0 !=(sel_ret = select(maxfd + 1, NULL, &set, NULL, &to)))
    {
        printf("%d\n", sel_ret);

        //perror("select");
        send(lfd, "hello", 6, 0);
        //perror("select");
    }

    close(lfd);
}
```

