#include <iostream>
#include <ucontext.h>
#include <unistd.h>
#include <vector>
#include <fcntl.h>

#define SIZE 16384 // because we can't get size after allocating

ucontext_t *uctx_func1;
ucontext_t *uctx_func2;
ucontext_t *uctx_main;
ucontext_t *uctx_disp;

std::vector<ucontext_t *> readyList;

void f1(void)
{
    while (1)
    {
        std::cout << "1\n"
                  << std::flush;
        sleep(1);
        std::cout << "Swap context: 1->disp\n";
        swapcontext(uctx_func1, uctx_disp);
    }
}

void f2(void)
{
    int ch;
    while (true)
    {
        std::cout << "2\n"
                  << std::flush;
        sleep(1);
        ch = getchar();
        if (ch == 10)
        {
            std::cout << "Enter pressed: 2->M\n";
            swapcontext(uctx_func2, uctx_main);
        }
        else
        {
            std::cout << "Swap context: 2->disp\n";
            swapcontext(uctx_func2, uctx_disp);
        }
    }
}

void disp(void)
{
    // let's create a queue of co-progs:
    ucontext_t *uctx_funca = uctx_func2;
    readyList = {uctx_func1};

    while (true)
    {
        std::cout << "Disp is choosing...\n";
        readyList.push_back(uctx_funca);
        uctx_funca = readyList.front();
        readyList.erase(readyList.begin());
        swapcontext(uctx_disp, uctx_funca);
    }
}

int main()
{
    // let's set NONBLOCK getchar():
    int flags = fcntl(STDIN_FILENO, F_GETFL);
    fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);

    // allocating stacks:
    char *func1_stack = new char[SIZE];
    char *func2_stack = new char[SIZE];
    char *disp_stack = new char[SIZE];

    // allocating size for descriptors
    uctx_func1 = new ucontext_t;
    uctx_func2 = new ucontext_t;
    uctx_disp = new ucontext_t;
    uctx_main = new ucontext_t;

    // creating coroutines

    // 1
    getcontext(uctx_func1);
    (*uctx_func1).uc_stack.ss_sp = (void *)func1_stack;
    (*uctx_func1).uc_stack.ss_size = SIZE;
    makecontext(uctx_func1, f1, 0);

    // 2
    getcontext(uctx_func2);
    (*uctx_func2).uc_stack.ss_sp = (void *)func2_stack;
    (*uctx_func2).uc_stack.ss_size = SIZE;
    makecontext(uctx_func2, f2, 0);

    // dispetcher
    getcontext(uctx_disp);
    (*uctx_disp).uc_stack.ss_sp = (void *)disp_stack;
    (*uctx_disp).uc_stack.ss_size = SIZE;
    makecontext(uctx_disp, disp, 0);

    // creating main context
    getcontext(uctx_main);

    std::cout << "Swap context: M->disp\n";
    swapcontext(uctx_main, uctx_disp);

    // deleting
    delete uctx_func1;
    delete uctx_func2;
    delete uctx_main;
    delete uctx_disp;

    delete[] func1_stack;
    delete[] func2_stack;
    delete[] disp_stack;

    // restore getchar() to default
    fcntl(STDIN_FILENO, F_SETFL, flags);

    std::cout << "The prog has finished\n";
    return 0;
}