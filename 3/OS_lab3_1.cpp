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
    sleep(1);
    printf("\x1b[31mThread 1 has started\x1b[0m\n");
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
        stat("OS_lab3_1.cpp", &buff);
        std::cout << "\nThread 1 is working...\n";

        size = sprintf(buf, "The message was: \x1b[36mSize of the .cpp file is: %ld\n\x1b[0m", buff.st_size);

        write(pipefd[1], buf, size),

            std::cout << "Thread 1 has wrote the message.\n";
        sleep(1);
    }

    close(pipefd[1]);
    printf("\x1b[31mThread 1 has been finished\x1b[0m\n");
    pthread_exit(NULL);
}

void *thread2(void *flag)
{
    printf("\x1b[31mThread 2 has started.\x1b[0m\n");

    while (*((int *)flag) != 0)
    {
        char buf[256];
        memset(buf, 0, sizeof(buf));
        int rv = read(pipefd[0], buf, sizeof(buf));
        std::cout << "\nThread 2 is working...\n";
        for (int i = 0; i < 256; i++)
        {
            if (buf[i] == '\0')
                break;
            std::cout << buf[i];
        }
        std::cout << "Thread 2 has finished.\n";
    }
    printf("\x1b[31m\nThread 2 has been finished\x1b[0m\n");
    pthread_exit(NULL);
}

int main()
{
    printf("Lab 3 (1)\n\n");
    pthread_t first, second;

    int *flag1 = new int;
    *flag1 = 1;
    int *flag2 = new int;
    *flag2 = 1;

    int rv = pipe(pipefd);

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