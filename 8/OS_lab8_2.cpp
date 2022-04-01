#include <iostream>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <cstdlib>

#include <stdlib.h>
#include <sys/select.h>
#include <termios.h>

#include <csignal>
#include <sys/types.h>
#include <sys/mman.h>
#include <fcntl.h>

#include <vector>
#include <string>
#include <algorithm>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

int client;
int flag1 = 1, flag2 = 1, flag3 = 1;
pthread_t _thread1, _thread2, _thread3;
char sndbuf[256];
char rcvbuf[256];

void *client_sent(void *arg)
{
    while (flag2)
    {
        int size = sprintf(sndbuf, "request");
        int sentcount = send(client, sndbuf, size, 0);
        if (sentcount == -1)
        {
            perror("send");
        }
        else
        {
            // send OK
        }
        for (int i = 0; i < size; i++)
        {
            std::cout << sndbuf[i];
        }
        std::cout << "\n";
        sleep(1);
        //создать запрос;
        //передать запрос в сокет;
        //вывести запрос на экран;
        //задержка на время 1 сек;
    }
    return NULL;
}
void *client_accept(void *arg)
{
    while (flag3)
    {
        int reccount = recv(client, rcvbuf, sizeof(rcvbuf), 0);
        if (reccount == -1)
        {
            perror("recv");
            sleep(1);
        }
        else if (reccount == 0)
        {
            sleep(1);
        }
        std::string s = rcvbuf;
        std::cout << s;
        std::cout << "\n";
    }
    return NULL;
}
void *client_connect(void *arg)
{
    struct sockaddr_in adr = {0};
    adr.sin_family = AF_INET;
    adr.sin_port = htons(31415);
    while (flag1)
    {
        int res = connect(client, (struct sockaddr *)&adr, sizeof(adr));
        if (res < 0)
        {
            perror("connect");
            continue;
        }

        int res = inet_pton(AF_INET, "127.0.0.1", &adr.sin_addr);
        if (res == 0)
        {
            std::cout << "fail!\n"
                      << "\n";
            continue;
        }
        else if (res = -1)
        {
            perror("inet_pton");
            continue;
        }
        else
        {
            pthread_create(&_thread2, NULL, &client_accept, NULL);
            pthread_create(&_thread3, NULL, &client_sent, NULL);
            break;
        }
    }
    return NULL;
}
int main()
{
    client = socket(AF_INET, SOCK_STREAM, 0);

    pthread_create(&_thread1, NULL, &client_connect, NULL);
    getchar();
    pthread_join(_thread1, NULL);
    pthread_join(_thread2, NULL);
    pthread_join(_thread3, NULL);

    shutdown(client, 0 | 1);
    close(client);

    return 0;
}