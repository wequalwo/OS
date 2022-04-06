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
#define SIZE 256

int flag = 1;
sem_t *sem_r;
sem_t *sem_w;
char *addr;
char *buf;

// void sig_handler(int signo)
// {
//     sem_close(sem_w);
//     sem_close(sem_r);
//     sem_unlink("bonjour.txt");
// }

void *wwrite(void *m)
{
    while (flag)
    {
        std::cout << "Prog 2 is waiting...\n";
        sem_wait(sem_w);
        std::cout << "Prog 2 is reading...\n";
        memcpy(buf, addr, strlen(addr));
        sem_post(sem_r);
        std::cout << "Prog 2 posted sem_r.\n";
        std::cout << "The message from shared mem: ";
        for (int i = 0; i < SIZE; i++)
        {
            std::cout << buf[i];
        }
        std::cout << "\n";
    }
    return NULL;
}

int main()
{
    //std::signal(SIGINT, sig_handler);

    int fd_shm = shm_open("/bonjour", O_CREAT | O_RDWR, 0644);

    sem_w = sem_open("write", O_CREAT, 0644, 0);
    sem_r = sem_open("read", O_CREAT, 0644, 0);

    buf = new char[SIZE];

    addr = (char *)mmap(0, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd_shm, 0);
    if (addr == MAP_FAILED)
    {
        std::cout << "There are some errors here \n";
    }
    sleep(3);
    std::cout << "all right\n";
    ftruncate(fd_shm, SIZE);

    pthread_t thread;
    pthread_create(&thread, NULL, &wwrite, NULL);

    getchar();
    flag = 0;
    pthread_join(thread, NULL);

    munmap(addr, SIZE);
    close(fd_shm);
    shm_unlink("bonjour.txt");

    delete[] buf;

    sem_close(sem_w);
    sem_close(sem_r);
    sem_unlink("/bonjour");

    return 0;
}