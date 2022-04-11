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
        std::cout << "1\n"
                  << std::flush;
        sleep(1);
        // передать управление программе 2
        std::cout << "Swap context: 1->2\n";
        swapcontext(&uctx_func1, &uctx_func2);
    }
}

void func2(void)
{
    int count = 0;
    while (1)
    {
        std::cout << "2\n" << std::flush;
        sleep(1);
        if (count > 3)
        {
            // передать управление сопрограмме М;
            std::cout << "Swap context: 2->M\n";
            swapcontext(&uctx_func2, &uctx_main);
        }
        else
        {
            std::cout << "Swap context: 2->1\n";
            swapcontext(&uctx_func2, &uctx_func1);
        }
        count++;
    }
}

int main()
{
    char *func1_stack = new char[16384];
    char *func2_stack = new char[16384];

    getcontext(&uctx_func1);
    uctx_func1.uc_stack.ss_sp = func1_stack;
    uctx_func1.uc_stack.ss_size = sizeof(func1_stack);
    makecontext(&uctx_func1, func1, 0);

    getcontext(&uctx_func2);
    uctx_func2.uc_stack.ss_sp = func2_stack;
    uctx_func2.uc_stack.ss_size = sizeof(func2_stack);
    makecontext(&uctx_func2, func2, 0);

    getcontext(&uctx_main);

    std::cout << "Swap context: M->1\n";
    swapcontext(&uctx_main, &uctx_func1);

    delete[] func1_stack;
    delete[] func2_stack;
    std::cout << "The prog has finished\n";
    return 0;
}