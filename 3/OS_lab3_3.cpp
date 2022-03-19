#include <iostream>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <cstring>

pthread_mutex_t mutex;
int pipefd[2];

void *thread1(void *flag)
{
    sleep(2);
    printf("\x1b[31mThread 1 have started\x1b[0m\n");
    FILE *fp;
    struct stat buff;
    if ((fp = fopen("OS_lab3_1.cpp", "rb")) == NULL)
    {
        printf("Cannot open file.\n");
        exit(1);
    }
    int size = 0;
    while (*((int *)flag) != 0)
    {
        char buf[256];
        int ret_val = stat("OS_lab3_1.cpp", &buff);

        if (ret_val != 0)
        {
            perror("stat");
            sleep(1);
            continue;
        }

        std::cout << "\nThread 1 is working...\n";

        size = sprintf(buf, "The message was: Size of the .cpp file is: %ld\n", buff.st_size);

        ret_val = write(pipefd[1], buf, size);
        if (ret_val == -1)
        {
            perror("write");
            sleep(1);
            continue;
        }

        std::cout << "Thread 1 have wrote the message.\n";
        sleep(2);
    }

    close(pipefd[1]);
    printf("\x1b[31mThread 1 have been finished\x1b[0m\n");
    pthread_exit(NULL);
}

void *thread2(void *flag)
{
    printf("\x1b[31mThread 2 have started.\x1b[0m\n");

    while (*((int *)flag) != 0)
    {
        char buf[256];
        memset(buf, 0, sizeof(buf));
        int rv = read(pipefd[0], buf, sizeof(buf));
        if (rv == -1)
        {
            perror("read");
            sleep(1);
            continue;
        }
        std::cout << "\nThread 2 is working...\n";
        for (int i = 0; i < 256; i++)
        {
            if (buf[i] == '\0')
                break;
            std::cout << buf[i];
        }
        std::cout << "Thread 2 have finished.\n";
        sleep(1);
    }
    close(pipefd[0]);
    printf("\x1b[31m\nThread 2 have been finished\x1b[0m\n");
    pthread_exit(NULL);
}

int main()
{
    printf("Lab 3 (3)\n\n");
    pthread_t first, second;

    int *flag1 = new int;
    *flag1 = 1;
    int *flag2 = new int;
    *flag2 = 1;

    int rv = pipe(pipefd);
    fcntl(pipefd[0], F_SETFL, O_NONBLOCK);
    fcntl(pipefd[1], F_SETFL, O_NONBLOCK);

    pthread_create(&first, NULL, &thread1, ((void *)flag1));
    pthread_create(&second, NULL, &thread2, ((void *)flag2));

    getchar();
    *flag1 = 0;
    *flag2 = 0;

    pthread_join(second, NULL);
    pthread_join(first, NULL);

    delete flag1;
    delete flag2;
    return 0;
}