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
    sleep(2);
    printf("\x1b[31mThread 2 have started\x1b[0m\n");

    while (flag != 0)
    {
        char buf[256];
        // std::cout << "\nThread 2 is working...\n";

        int result = mq_receive(mq, buf, sizeof(buf), 0);
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
        std::cout << "\n";
        std::cout << "Thread 2 have written the message.\n";
        sleep(1);
    }
    return NULL;
}

int main()
{
    mq = mq_open("/mq", O_CREAT | O_RDONLY | O_NONBLOCK, 0644, NULL);
    pthread_t th;
    pthread_create(&th, NULL, &thread, NULL);
    getchar();
    flag = 1;
    pthread_join(th, NULL);
    int mq_close(mq);
    int mq_unlink(mq);
    return 0;
}