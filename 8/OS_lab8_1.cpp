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

int server;                       // id listener socket
pthread_mutex_t queue_mutex, out_mutex;   // mutexes id-s
std::vector<std::string> msglist; // request queue
// std::vector<std::string> seglist; // sent queue
char sndbuf[SIZE];
char rcvbuf[SIZE];
int flag_receive = 0; // completion flag for receiving requests
int flag_request = 0; // the flag for the completion of the request processing thread and the transmission of responses;
int flag_connect = 0; // connection waiting thread termination flag

pthread_t _thread_receive, _thread_request, _thread_connect;
int fd;

int out(std::string str)
{
    pthread_mutex_lock(&out_mutex);
    std::cout << str << "\n";
    pthread_mutex_unlock(&out_mutex);
    return 1;
}

void *get_receive(void *arg)
{
    out("Receive has started.\n");
    while (!flag_receive)
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
            pthread_mutex_lock(&queue_mutex);
            std::string s = rcvbuf;
            msglist.push_back(s);
            pthread_mutex_unlock(&queue_mutex);
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
    while (!flag_request)
    {
        pthread_mutex_lock(&queue_mutex);
        if (!msglist.empty())
        {
            if (msglist.size() == 0)
                continue;
            std::string S = msglist.back();
            out("Client's message: \"" + S + "\"");
            msglist.pop_back();
            pthread_mutex_unlock(&queue_mutex);

            std::memset(sndbuf, '\0', SIZE);
            stat("OS_lab3_1.cpp", &buff);

            size = sprintf(sndbuf, "\x1b[36mSize of the .cpp file is: %ld\x1b[0m, count = %d", buff.st_size, count);
            count++;

            pthread_mutex_lock(&out_mutex);
            std::cout << "Message to client will be: \"";
            for (int i = 0; i < size; i++)
            {
                std::cout << sndbuf[i];
            }
            std::cout << "\"\n\n";
            pthread_mutex_unlock(&out_mutex);

            int sentcount = send(fd, sndbuf, size, 0);
            if (sentcount == -1)
            {
                perror("send");
            }
        }
        else
        {
            pthread_mutex_unlock(&queue_mutex);
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
            sleep(1);
            continue;
        }
        else
        {
            out("Client has been connected!");
            pthread_create(&_thread_receive, NULL, &get_receive, NULL);
            pthread_create(&_thread_request, NULL, &get_request, NULL);
            break;
        }
    }
    return NULL;
}

void sig_handler(int signo)
{
    std::cout << "\nForce exit...\n";
    shutdown(server, SHUT_RDWR);
    close(server);
    pthread_mutex_destroy(&queue_mutex);
    pthread_mutex_destroy(&out_mutex);
    exit(0);
}

int main()
{
    signal(SIGPIPE, sig_handler);

    std::cout << "v.5.1\n";
    pthread_mutex_init(&queue_mutex, NULL);
    pthread_mutex_init(&out_mutex, NULL);

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
    fcntl(server, F_SETFL, O_NONBLOCK);

    int res = bind(server, (struct sockaddr *)&adr, sizeof(adr));
    if (res == -1)
    {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    res = listen(server, 4);
    if (res == -1)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    res = pthread_create(&_thread_connect, NULL, &get_connect, NULL);
    if (res == -1)
    {
        perror("pthread_create");
        exit(EXIT_FAILURE);
    }

    res = getchar();
    if (res == -1)
    {
        perror("getchar");
    }
    flag_connect = 1;
    flag_receive = 1;
    flag_request = 1;

    pthread_join(_thread_connect, NULL);
    pthread_join(_thread_request, NULL);
    pthread_join(_thread_receive, NULL);
    shutdown(server, SHUT_RDWR);
    pthread_mutex_destroy(&queue_mutex);
    pthread_mutex_destroy(&out_mutex);
    close(server);
    return 0;
}