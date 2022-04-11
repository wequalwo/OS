#include <iostream>
#include <ucontext.h>
#include <unistd.h>

ucontext_t uctx_func1;
ucontext_t uctx_func2;
ucontext_t uctx_main;

void func1(void)
{
    while (1)
    {
        std::cout << "1" << std::flush;
        sleep(1);
        // передать управление программе 2
        swapcontext(&uctx_func1, &uctx_func2);
    }
}

void func2(void)
{
    int count = 0;
    while (1)
    {
        std::cout << "2" << std::flush;
        sleep(1);
        if (count > 3)
        {
            // передать управление сопрограмме М;
            swapcontext(&uctx_func2, &uctx_main);
        }
        else
        {
            // передать управление сопрограмме 1;
            swapcontext(&uctx_func2, &uctx_func1);
        }
        count++;
    }
}

int main()
{
    // char *func1_stack = new char[16384];
    // char *func2_stack = new char[16384];

    char func1_stack[16384];
    char func2_stack[16384];

    getcontext(&uctx_func1);
    uctx_func1.uc_stack.ss_sp = func1_stack;
    uctx_func1.uc_stack.ss_size = sizeof(func1_stack);
    makecontext(&uctx_func1, func1, 0);

    getcontext(&uctx_func2);
    uctx_func2.uc_stack.ss_sp = func2_stack;
    uctx_func2.uc_stack.ss_size = sizeof(func2_stack);
    makecontext(&uctx_func2, func1, 0);

    getcontext(&uctx_main);
    swapcontext(&uctx_main, &uctx_func1);
    // delete[] func1_stack;
    // delete[] func2_stack;
    return 0;
}