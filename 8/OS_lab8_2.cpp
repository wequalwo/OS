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

#include <signal.h>
typedef void (*sighandler_t)(int);

#define SIZE 256

pthread_mutex_t mutex;

int client;
int flag_connect = 1, flag_sent = 1, flag_accept = 1;
pthread_t _thread_connect, _thread_accept, _thread_sent;
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
    while (flag_sent)
    {
        std::memset(sndbuf, '\0', SIZE);
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
    while (flag_accept)
    {
        std::memset(rcvbuf, '\0', SIZE);
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
    while (flag_connect)
    {
        int res = connect(client, (struct sockaddr *)&adr, sizeof(adr));
        if (res < 0)
        {
            perror("connect");
            sleep(1);
            continue;
        }

        pthread_create(&_thread_accept, NULL, &client_accept, NULL);
        pthread_create(&_thread_sent, NULL, &client_sent, NULL);
        out("Server has been connected!\n");
        return NULL;
    }
    return NULL;
}

void sig_handler(int signo)
{
    std::cout << "\nForce exit...\n";
    shutdown(client, SHUT_RDWR);
    close(client);
    pthread_mutex_destroy(&mutex);
    exit(0);
}

int main()
{
    signal(SIGPIPE, sig_handler);

    pthread_mutex_init(&mutex, NULL);
    client = socket(AF_INET, SOCK_STREAM, 0);

    int optval = 1;
    setsockopt(client, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
    fcntl(client, F_SETFL, O_NONBLOCK);

    pthread_create(&_thread_connect, NULL, &client_connect, NULL);

    getchar();
    flag_connect = flag_sent = flag_accept = 0;

    pthread_join(_thread_connect, NULL);
    pthread_join(_thread_accept, NULL);
    pthread_join(_thread_sent, NULL);

    shutdown(client, SHUT_RDWR);
    close(client);
    pthread_mutex_destroy(&mutex);
    return 0;
}