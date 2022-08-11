  #include <stdio.h>
  #include <stdlib.h>
  #include <unistd.h>
  #include <sys/types.h>
  #include <sys/socket.h>
  #include <string.h>
  #include <errno.h>
  #include <netinet/in.h>
  #include <arpa/inet.h>

    int main()
    {
        int listen_sock = socket(AF_INET, SOCK_STREAM, 0);
        if (0 <= listen_sock)
        {
            struct sockaddr_in stServerAddr;
            stServerAddr.sin_family = AF_INET;
            stServerAddr.sin_addr.s_addr = htonl(INADDR_ANY);
            stServerAddr.sin_port = htons(55555);
            if ((0 == bind(listen_sock, (struct sockaddr *)&stServerAddr, sizeof(stServerAddr))) &&
                (0 == listen(listen_sock, 10)))
            {
                struct sockaddr_in stClientAddr;
                socklen_t AddrLen = sizeof(stClientAddr);
                int data_sock = -1;
                while (0 <= (data_sock = accept(listen_sock, (struct sockaddr *)&stClientAddr, &AddrLen)))
                {
                    char szBuff[256];
                    int recv_len = 0;
                    while (0 < (recv_len = recv(data_sock, szBuff, sizeof(szBuff), 0)))
                    {
                        send(data_sock, szBuff, recv_len, 0);
                    }
                    close(data_sock);
                }
            }
            else
            {
                perror("bind:");
            }
        }
        else
        {
            perror("create listen socket:");
        }
        return -1;
    }
