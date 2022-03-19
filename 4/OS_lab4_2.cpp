#include <iostream>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <cstring>
#include <time.h>
#include <stdlib.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
    std::cout << "Prog 2 pid: " << getpid();
    std::cout << "\nProg 2 ppid: " << getppid() << "\n";

    pid_t pid = fork();
    if (pid == -1)
    {
        std::cerr << "\nInvalid pid\n";
        return -1;
    }

    if (pid == 0)
    {
        std::cout << "Child process pid is " << getpid() << "\n";
        execv("OS_lab4_1", argv);
    }
    if (pid > 0)
    {
        int status;
        std::cout << "Main pid is " << getpid() << "\n";
        while (waitpid(pid, &status, WNOHANG) == 0)
        {
            std::cout << "Main is waiting\n";
            sleep(1);
        }
        std::cout << "Prog 1 exit status is " << WEXITSTATUS(status) << "\n";
    }

    return 0;
}