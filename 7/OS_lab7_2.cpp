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
#define my_mq_maxmsg 47 //пробуем увеличить очередь

int flag = 1;
mqd_t mq;

void *thread2(void *arg)
{
    printf("\x1b[31mThread 2 has started\x1b[0m\n");

    while (flag != 0)
    {
        char buf[msg_size];
        int result = mq_receive(mq, buf, msg_size, 0);
        if (result == -1)
        {
            perror("mq_receive");
            sleep(1);
            continue;
        }
        for (auto i : buf)
        {
            std::cout << i;
        }
        std::cout << "Thread 2 has written the message.\n\n";
        sleep(1);
    }
    std::cout << "Thread 2 has finished\n";
    return NULL;
}

int main()
{
    struct mq_attr attr = {0, my_mq_maxmsg, msg_size, 0};
    flag = 1;
    mq = mq_open("/mq", O_CREAT | O_RDONLY | O_NONBLOCK, 0644, &attr);
    int t = mq_getattr(mq, &attr);
    std::cout << "mq_maxmsg = " << attr.mq_maxmsg << "\n";
    
    pthread_t th;
    std::cout << "Prog 2 is ready\n";
    pthread_create(&th, NULL, &thread2, NULL);
    getchar();
    flag = 0;
    pthread_join(th, NULL);
    mq_close(mq);
    mq_unlink("/mq");
    return 0;
}