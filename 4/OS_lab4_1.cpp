#include <iostream>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <cstring>

int main(int argc, char *argv[])
{   
    std::cout << "Prog 1 pid: " << getpid();
    std::cout << "\nProg 1 ppid: " << getppid() << "\n";

    for(int i = 0; i < argc; i++)
    {
        std::cout << argv[i] << "\n";
        sleep(1);
    }
    
    std::cout << "Prog 1 finished!\n";
    return 1;
}