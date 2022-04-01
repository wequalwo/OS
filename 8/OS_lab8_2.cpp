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

#include <cstring>
#include <vector>
#include <string>
#include <algorithm>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#define SIZE 256

pthread_mutex_t mutex;

int client;
int flag1 = 1, flag2 = 1, flag3 = 1;
pthread_t _thread1, _thread2, _thread3;
char sndbuf[SIZE];
char rcvbuf[SIZE];

int out(std::string str)
{
    pthread_mutex_lock(&mutex);
    std::cout << str << "\n";
    pthread_mutex_unlock(&mutex);
    return 1;
}

void *client_sent(void *arg)
{
    int count = 0;
    while (flag2)
    {
        std::memset(sndbuf,'\0', SIZE);
        int size = sprintf(sndbuf, "attempt %d", count);
        int sentcount = send(client, sndbuf, size, 0);
        if (sentcount == -1)
        {
            perror("send");
        }
        else
        {
            pthread_mutex_lock(&mutex);
            std::cout << "The message was: \"";
            for (int i = 0; i < size; i++)
            {
                std::cout << sndbuf[i];
            }
            std::cout << "\"\n";
            pthread_mutex_unlock(&mutex);
        }
        sleep(1);
        count++;
    }
    return NULL;
}

void *client_accept(void *arg)
{
    while (flag3)
    {
        std::memset(rcvbuf,'\0', SIZE);
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
        out("Server answer was: \"" + s + "\"\n");
        sleep(1);
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
            sleep(1);
            continue;
        }

        pthread_create(&_thread2, NULL, &client_accept, NULL);
        pthread_create(&_thread3, NULL, &client_sent, NULL);
        out("Server has been connected!\n");
        return NULL;
    }
    return NULL;
}

int main()
{
    pthread_mutex_init(&mutex, NULL);
    client = socket(AF_INET, SOCK_STREAM, 0);
    fcntl(client, F_SETFL, O_NONBLOCK);
    int optval = 1;
    setsockopt(client, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

    pthread_create(&_thread1, NULL, &client_connect, NULL);

    getchar();
    flag1 = flag2 = flag3 = 0;
    pthread_join(_thread1, NULL);
    pthread_join(_thread2, NULL);
    pthread_join(_thread3, NULL);

    shutdown(client, SHUT_RDWR);
    close(client);
    pthread_mutex_destroy(&mutex);
    return 0;
}