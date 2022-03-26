#include <iostream>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <cstring>
#include <fstream>
#include <semaphore.h>
#include <cstdlib>

#include <stdlib.h>
#include <sys/select.h>
#include <termios.h>

#include <csignal>
#include <sys/types.h>
#include <sys/mman.h>
#include <fcntl.h>

#include <mqueue.h>
#define msg_size 256

int flag = 1;
mqd_t mq;

void *thread1(void *arg)
{
    int count = 0;
    printf("\x1b[31mThread 1 has started\x1b[0m\n");
    FILE *fp;
    struct stat buff;
    if ((fp = fopen("OS_lab7_2.cpp", "r")) == NULL)
    {
        printf("Cannot open file.\n");
        exit(1);
    }
    int size = 0;
    while (flag != 0)
    {
        char buf[msg_size];
        int val = stat("OS_lab7_1.cpp", &buff);
        if (val != 0)
        {
            perror("stat");
            sleep(1);
            continue;
        }
        size = sprintf(buf, "The message was: \x1b[36mSize of the .cpp file is: %ld\x1b[0m, count = %d\n", buff.st_size, count);
        for(int i = 0; i < size; i++)
        {
            std::cout << buf[i];
        }
        int result = mq_send(mq, buf, size, 0);
        if (result == -1)
        {
            perror("mq_send");
            sleep(1);
            continue;
        }
        std::cout << "Thread 1 has wrote the message.\n\n";
        sleep(1);
        count++;
    }
    std::cout << "Thread 1 has finished\n";
    return NULL;
}

int main()
{
    flag = 1;
    struct mq_attr attr = {0, 10, msg_size, 0};
    mq = mq_open("/mq", O_CREAT | O_WRONLY | O_NONBLOCK, 0644, &attr);
    pthread_t th1;
    std::cout << "Prog 1 is ready\n";
    pthread_create(&th1, NULL, &thread1, NULL);
    getchar();
    flag = 0;
    pthread_join(th1, NULL);
    mq_close(mq);
    mq_unlink("/mq");
    return 0;
}