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

int server;                       // id listener socket
pthread_mutex_t mutex1, mutex2;   // mutexes id-s
std::vector<std::string> msglist; // request queue
// std::vector<std::string> seglist; // sent queue
char sndbuf[SIZE];
char rcvbuf[SIZE];
int flag_receive = 0; // completion flag for receiving requests
int flag_request = 0; // the flag for the completion of the request processing thread and the transmission of responses;
int flag_connect = 0; // connection waiting thread termination flag

pthread_t _receive, _request, _connect;
int fd;

int out(std::string str)
{
    pthread_mutex_lock(&mutex2);
    std::cout << str << "\n";
    pthread_mutex_unlock(&mutex2);
    return 1;
}

void *get_receive(void *arg)
{
    out("Receive has started.\n");
    while (true)
    {
        std::memset(rcvbuf, '\0', SIZE);
        int reccount = recv(fd, rcvbuf, sizeof(rcvbuf), 0);
        if (reccount == -1)
        {
            perror("recv");
            sleep(1);
        }
        else if (reccount == 0)
        {
            out("\nEmpty queue...");
            sleep(1);
        }
        else
        {
            pthread_mutex_lock(&mutex1);
            std::string s = rcvbuf;
            msglist.push_back(s);
            pthread_mutex_unlock(&mutex1);
        }
        sleep(1);
    }
    return NULL;
}

void *get_request(void *arg)
{
    out("Request has started");
    int count = 0;

    FILE *fp;
    struct stat buff;
    if ((fp = fopen("OS_lab8_1.cpp", "rb")) == NULL)
    {
        perror("fopen");
        exit(EXIT_FAILURE);
    }
    int size = 0;
    while (true)
    {
        pthread_mutex_lock(&mutex1);
        if (!msglist.empty())
        {
            if (msglist.size() == 0)
                continue;
            std::string S = msglist.back();
            out("Client's message: \"" + S + "\"");
            msglist.pop_back();
            pthread_mutex_unlock(&mutex1);

            std::memset(sndbuf, '\0', SIZE);
            stat("OS_lab3_1.cpp", &buff);

            size = sprintf(sndbuf, "\x1b[36mSize of the .cpp file is: %ld\x1b[0m, count = %d", buff.st_size, count);
            count++;

            pthread_mutex_lock(&mutex2);
            std::cout << "Message to client will be: \"";
            for (int i = 0; i < size; i++)
            {
                std::cout << sndbuf[i];
            }
            std::cout << "\"\n\n";
            pthread_mutex_unlock(&mutex2);

            int sentcount = send(fd, sndbuf, size, 0);
            if (sentcount == -1)
            {
                perror("send");
            }
        }
        else
        {
            pthread_mutex_unlock(&mutex1);
            out("empty queue");
        }
        sleep(1);
    }
    return NULL;
}

void *get_connect(void *arg)
{
    struct sockaddr_in remote_adr = {0};
    socklen_t adrlen = sizeof(remote_adr);
    while (!flag_connect)
    {
        fd = accept(server, (struct sockaddr *)&remote_adr, &adrlen);
        if (fd < 0)
        {
            out("Server is waiting...");
            continue;
        }
        else
        {
            out("Client has been connected!");
            pthread_create(&_receive, NULL, &get_receive, NULL);
            pthread_create(&_request, NULL, &get_request, NULL);
            break;
        }
    }
    return NULL;
}

int main()
{
    fcntl(server, F_SETFL, O_NONBLOCK);
    std::cout << "v.3.1ss\n";
    pthread_mutex_init(&mutex1, NULL);
    pthread_mutex_init(&mutex2, NULL);

    server = socket(AF_INET, SOCK_STREAM, 0);
    if (server == -1)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    struct sockaddr_in adr = {0};
    adr.sin_family = AF_INET;
    adr.sin_port = htons(31415);
    int optval = 1;

    setsockopt(server, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

    int res = bind(server, (struct sockaddr *)&adr, sizeof(adr));
    if (res == -1)
    {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    res = listen(server, 1);
    if (res == -1)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    res = pthread_create(&_connect, NULL, &get_connect, NULL);
    if (res == -1)
    {
        perror("pthread_create");
        exit(EXIT_FAILURE);
    }

    while (getchar() == -1)
        ;
    flag_connect = 1;
    flag_receive = 1;
    flag_request = 1;

    pthread_join(_connect, NULL);
    pthread_join(_request, NULL);
    pthread_join(_receive, NULL);
    shutdown(server, SHUT_RDWR);
    pthread_mutex_destroy(&mutex1);
    pthread_mutex_destroy(&mutex2);
    close(server);
    return 0;
}