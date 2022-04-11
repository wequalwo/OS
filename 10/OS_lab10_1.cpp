#include <iostream>
#include <ucontext.h>
#include <unistd.h>
#include <vector>

#define SIZE 16384

ucontext_t *uctx_func1;
ucontext_t *uctx_func2;
ucontext_t *uctx_main;

void func1(void)
{
    while (1)
    {
        std::cout << "1\n"
                  << std::flush;
        sleep(1);
        std::cout << "Swap context: 1->2\n";
        swapcontext(uctx_func1, uctx_func2);
    }
}

void func2(void)
{
    int count = 0;
    while (1)
    {
        std::cout << "2\n"
                  << std::flush;
        sleep(1);
        if (count >= 2)
        {
            std::cout << "Swap context: 2->M\n";
            swapcontext(uctx_func2, uctx_main);
        }
        else
        {
            std::cout << "Swap context: 2->1\n";
            swapcontext(uctx_func2, uctx_func1);
        }
        count++;
    }
}

int main()
{
    char *func1_stack = new char[SIZE];
    char *func2_stack = new char[SIZE];

    uctx_func1 = new ucontext_t;
    uctx_func2 = new ucontext_t;
    uctx_main = new ucontext_t;

    getcontext(uctx_func1);
    (*uctx_func1).uc_stack.ss_sp = (void *)func1_stack;
    (*uctx_func1).uc_stack.ss_size = SIZE;
    makecontext(uctx_func1, func1, 0);

    getcontext(uctx_func2);
    (*uctx_func2).uc_stack.ss_sp = (void *)func2_stack;
    (*uctx_func2).uc_stack.ss_size = SIZE;
    makecontext(uctx_func2, func2, 0);

    getcontext(uctx_main);

    std::cout << "Swap context: M->1\n";
    swapcontext(uctx_main, uctx_func1);

    delete uctx_func1;
    delete uctx_func2;
    delete uctx_main;

    delete[] func1_stack;
    delete[] func2_stack;

    std::cout << "The prog has finished\n";
    return 0;
}