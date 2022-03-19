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

void *wwrite(void *m)
{
    struct stat buff;
    int count = 0;
    while (flag)
    {
        stat("OS_lab6_1.cpp", &buff);

        int size = sprintf(buf, "\x1b[36mSize of the .cpp file is: %ld; \x1b[0mIteration is %d", buff.st_size, count);
        std::cout << "Prog 1 is writing...\n";
        memcpy(addr, buf, size);

        std::cout << "The message to write: ";
        for (int i = 0; i < SIZE; i++)
        {
            std::cout << buf[i];
        }
        std::cout << "\n";

        sem_post(sem_w);
        std::cout << "Prog 1 posted sem_w...\n";
        sem_wait(sem_r);
        std::cout << "Prog 1 is waiting...\n";
        sleep(1);
        count++;
    }
    return NULL;
}

int main()
{

    //std::signal(SIGINT, sig_handler);
    int fd_shm = shm_open("bonjour.txt", O_CREAT | O_RDWR, 0644);

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
    sem_unlink("bonjour.txt");

    return 0;
}