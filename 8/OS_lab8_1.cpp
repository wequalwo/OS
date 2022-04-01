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
    while (!flag_receive)
    {
        int reccount = recv(server, rcvbuf, sizeof(rcvbuf), 0);
        if (reccount == -1)
        {
            perror("recv");
            sleep(1);
        }
        else if (reccount == 0)
        {
            sleep(1);
        }
        else
        {
            pthread_mutex_lock(&mutex1);
            std::string s = rcvbuf;
            msglist.push_back(s);
            pthread_mutex_unlock(&mutex1);
        }
    }
    return NULL;
}

void *get_request(void *arg)
{
    while (!flag_request)
    {
        int i = 0;
        pthread_mutex_lock(&mutex1);
        if (!msglist.empty())
        {                                   //очередь запросов не пуста
            std::string S = msglist.back(); //получаем первый в очереди запрос
            msglist.pop_back();             //удаляем его из очереди
            pthread_mutex_unlock(&mutex1);
            //выполняем функцию, которую требует задание;
            //Например, uname.
            //Функция возвращает структуру из нескольких полей.
            //Берем любое поле, превращаем его в массив символов.
            //Назовем массив sndbuf.
            //Добавляете к нему запрос (проверка очередности запрос-ответ.
            //Передаем его вызовом:
            int size = sprintf(sndbuf, "N = : %d", i);
            int sentcount = send(server, sndbuf, size, 0);
            if (sentcount == -1)
            {
                perror("send");
            }
            else
            {
                // send OK
            }
        }
        else
        { //очередь пуста
            pthread_mutex_unlock(&mutex1);
            sleep(1);
        }
        // прочитать запрос из очереди на обработку;
        // выполнить заданную функцию;
        // передать ответ в сокет;
        // вывести результат на экран;
        i++;
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
            pthread_create(&_receive, NULL, &get_receive, NULL);
            pthread_create(&_request, NULL, &get_request, NULL);
            std::cout << "connected: " << fd << "\n";
            break;
        }
    }
    return NULL;
}

int main()
{

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

    getchar();
    flag_connect = 1;
    flag_receive = 1;
    flag_request = 1;
    pthread_join(_connect, NULL);
    pthread_join(_request, NULL);
    pthread_join(_receive, NULL);
    shutdown(server, 0 | 1);
    pthread_mutex_destroy(&mutex1);
    // pthread_mutex_destroy(&mutex2);
    close(server);
    return 0;
}