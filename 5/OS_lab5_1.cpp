#include <iostream>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <cstring>

#include <fstream>
#include <semaphore.h>
#include <cstdlib>

#include <stdlib.h>
#include <sys/select.h>
#include <termios.h>

struct termios orig_termios;

void reset_terminal_mode()
{
    tcsetattr(0, TCSANOW, &orig_termios);
}

void set_conio_terminal_mode()
{
    struct termios new_termios;

    /* take two copies - one for now, one for later */
    tcgetattr(0, &orig_termios);
    memcpy(&new_termios, &orig_termios, sizeof(new_termios));

    /* register cleanup handler, and set the new terminal mode */
    atexit(reset_terminal_mode);
    cfmakeraw(&new_termios);
    tcsetattr(0, TCSANOW, &new_termios);
}

int kbhit()
{
    struct timeval tv = {0L, 0L};
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(0, &fds);
    return select(1, &fds, NULL, NULL, &tv) > 0;
}


int main()
{
    sem_t *sem = sem_open("bonjour.txt", O_CREAT, 0644, 1);
    std::ofstream fout;
    fout.open("bonjour.txt", std::ofstream::app);
    

    while (!kbhit())
    {
        
        sem_wait(sem);
        for (int i = 0; i < 5; i++)
        {
            printf("1\n");
            fout << "1" << std::flush;
            sleep(1);
        }
       
        sem_post(sem);
        sleep(1);
    }
    fout.close();
    sem_close(sem);
    sem_unlink("bonjour.txt");
    return 0;
}