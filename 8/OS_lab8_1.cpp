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

int server;                       // id listener socket
pthread_mutex_t mutex1;           //, mutex2;   // mutexes id-s
std::vector<std::string> msglist; // request queue
// std::vector<std::string> seglist; // sent queue
char sndbuf[256];
char rcvbuf[256];
int flag_receive = 0; // completion flag for receiving requests
int flag_request = 0; // the flag for the completion of the request processing thread and the transmission of responses;
int flag_connect = 0; // connection waiting thread termination flag

pthread_t _receive, _request, _connect;
int fd;

void *get_receive(void *arg)
{
    std::cout << "receive has started\n";
    while (true)
    {
        int reccount = recv(fd, rcvbuf, sizeof(rcvbuf), 0);
        if (reccount == -1)
        {
            perror("recv");
            sleep(1);
        }
        else if (reccount == 0)
        {
            std::cout << "\nreccount == 0";
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
    std::cout << "request has started\n";
    int count = 0;
    while (true)
    {
        pthread_mutex_lock(&mutex1);
        if (!msglist.empty()) // очередь запросов не пуста
        {
            if (msglist.size() == 0)
                continue;
            std::string S = msglist.back(); //получаем первый в очереди запрос
            std::cout << "Client's message: \"" << S << "\"\n";
            msglist.pop_back(); //удаляем его из очереди
            pthread_mutex_unlock(&mutex1);
            //выполняем функцию, которую требует задание;
            //Например, uname.
            //Функция возвращает структуру из нескольких полей.
            //Берем любое поле, превращаем его в массив символов.
            //Назовем массив sndbuf.
            //Добавляете к нему запрос (проверка очередности запрос-ответ.
            //Передаем его вызовом:
            int size = sprintf(sndbuf, "N = %d", count);
            count++;
            std::cout << "Message to client will be: \"";
            for (int i = 0; i < size; i++)
            {
                std::cout << sndbuf[i];
            }
            std::cout << "\"\n";
            int sentcount = send(fd, sndbuf, size, 0);
            if (sentcount == -1)
            {
                std::cout << "sentcount == -1\n"
                          << std::flush;
                perror("send");
            }
        }
        else
        {
            std::cout << "\nempty queue\n";
            pthread_mutex_unlock(&mutex1);
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
            std::cout << "server is waiting...\n";
            continue;
        }
        else
        {
            std::cout << "connected: " << fd << "\n";
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
    std::cout << "v.2.7\n";
    pthread_mutex_init(&mutex1, NULL);
    // pthread_mutex_init(&mutex2, NULL);

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
    // pthread_mutex_destroy(&mutex2);
    close(server);
    return 0;
}