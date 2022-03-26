#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
pthread_mutex_t mutex;

void *thread1(void *flag)
{
    timespec tp;
    printf("\x1b[31mThread 1 has started\x1b[0m\n");
    while (*((int *)flag) != 0)
    {
        clock_gettime(CLOCK_REALTIME, &tp);
        tp.tv_sec += 2;
        while (pthread_mutex_timedlock(&mutex, &tp) != 0)
        {
            printf("\x1b[31mTimed thread1 failed!\x1b[0m\n");
            tp.tv_sec += 2;
        }
        printf("\x1b[32mThread 1 has locked the mutex\x1b[0m\n");
        for (int i = 0; i < 5; i++)
        {
            printf("1\n");
            sleep(1);
        }
        pthread_mutex_unlock(&mutex);
        printf("\x1b[32mThread 1 has unlocked the mutex\x1b[0m\n");
        sleep(2);
    }
    printf("\x1b[31mThread 1 has been finished\x1b[0m\n");
    pthread_exit(NULL);
}

void *thread2(void *flag)
{
    timespec tp;
    printf("\x1b[31mThread 2 has started\x1b[0m\n");

    while (*((int *)flag) != 0)
    {
        clock_gettime(CLOCK_REALTIME, &tp);
        tp.tv_sec += 2;
        while (pthread_mutex_timedlock(&mutex, &tp) != 0)
        {
            printf("\x1b[31mTimed thread2 failed!\x1b[0m\n");
            tp.tv_sec += 2;
        }

        printf("\x1b[32mThread 2 has locked the mutex\x1b[0m\n");
        for (int i = 0; i < 5; i++)
        {
            printf("2\n");
            sleep(1);
        }
        pthread_mutex_unlock(&mutex);
        printf("\x1b[32mThread 2 has unlocked the mutex\x1b[0m\n");
        sleep(1);
    }

    printf("\x1b[31m\nThread 2 has been finished\x1b[0m\n");
    pthread_exit(NULL);
}

int main()
{
    printf("Timedlock prog (3). Wait_time == 2s\n\n");

    pthread_t first, second;

    pthread_mutex_init(&mutex, NULL);

    int *flag1 = new int;
    *flag1 = 1;
    int *flag2 = new int;
    *flag2 = 1;

    pthread_create(&first, NULL, &thread1, ((void *)flag1));
    pthread_create(&second, NULL, &thread2, ((void *)flag2));

    getchar();
    *flag1 = 0;
    *flag2 = 0;

    pthread_join(second, NULL);
    pthread_join(first, NULL);

    pthread_mutex_destroy(&mutex);
    delete flag1;
    delete flag2;
    return 0;
}