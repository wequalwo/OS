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

int flag;
mqd_t mq;

void *thread(void *flag)
{
    printf("\x1b[31mThread 1 have started\x1b[0m\n");
    FILE *fp;
    struct stat buff;
    if ((fp = fopen("OS_lab7_1.cpp", "rb")) == NULL)
    {
        printf("Cannot open file.\n");
        exit(1);
    }
    int size = 0;
    while (*((int *)flag) != 0)
    {
        char buf[256];
        int val = stat("OS_lab7_1.cpp", &buff);

        if (val != 0)
        {
            perror("stat");
            sleep(1);
            continue;
        }

        size = sprintf(buf, "The message was: Size of the .cpp file is: %ld\n", buff.st_size);

        int result = mq_send(mq, buf, size, 0);
        if (result == -1)
        {
            perror("mq_send");
            sleep(1);
            continue;
        }
        std::cout << "Thread 1 have wrote the message.\n";
        sleep(1);
    }
    return NULL;
}

int main()
{
    mq = mq_open("/mq", O_CREAT | O_WRONLY | O_NONBLOCK, 0644, NULL);
    pthread_t th;
    pthread_create(&th, NULL, &thread, NULL);
    getchar();
    flag = 1;

    pthread_join(th, NULL);
    int mq_close(mq);
    int mq_unlink(mq);
    return 0;
}