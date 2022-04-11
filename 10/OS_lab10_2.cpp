#include <iostream>
#include <ucontext.h>
#include <unistd.h>
#include <vector>

#define SIZE 16384
#define I 5

ucontext_t *uctx_func1;
ucontext_t *uctx_func2;
ucontext_t *uctx_main;
ucontext_t *uctx_disp;

std::vector<ucontext_t *> readyList;

void func1(void)
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

void func2(void)
{
    while (1)
    {
        std::cout << "2\n"
                  << std::flush;
        sleep(1);
        std::cout << "Swap context: 2->disp\n";
        swapcontext(uctx_func2, uctx_disp);
    }
}

void disp(void)
{
    int count = 0;

    ucontext_t *uctx_funca = uctx_func2;
    readyList = {uctx_func1};
    while (count < I)
    {
        std::cout << "Disp is choosing\n";
        readyList.push_back(uctx_funca);    //приостанавливаемую сопрограмму - в конец очереди
        uctx_funca = readyList.front();     //получаем новую сопрограмму – первую в очереди
        readyList.erase(readyList.begin()); //исключаем новую сопрограмму из очереди
        swapcontext(uctx_disp, uctx_funca); //передаем управление новой сопрограмме.
        count++;
    }
    swapcontext(uctx_disp, uctx_main);
}

int main()
{
    char *func1_stack = new char[SIZE];
    char *func2_stack = new char[SIZE];
    char *disp_stack = new char[SIZE];

    uctx_func1 = new ucontext_t;
    uctx_func2 = new ucontext_t;
    uctx_disp = new ucontext_t;
    uctx_main = new ucontext_t;

    getcontext(uctx_func1);
    (*uctx_func1).uc_stack.ss_sp = (void *)func1_stack;
    (*uctx_func1).uc_stack.ss_size = SIZE;
    makecontext(uctx_func1, func1, 0);

    getcontext(uctx_func2);
    (*uctx_func2).uc_stack.ss_sp = (void *)func2_stack;
    (*uctx_func2).uc_stack.ss_size = SIZE;
    makecontext(uctx_func2, func2, 0);

    getcontext(uctx_disp);
    (*uctx_disp).uc_stack.ss_sp = (void *)disp_stack;
    (*uctx_disp).uc_stack.ss_size = SIZE;
    makecontext(uctx_disp, disp, 0);

    getcontext(uctx_main);

    std::cout << "Swap context: M->disp\n";
    swapcontext(uctx_main, uctx_disp);

    delete uctx_func1;
    delete uctx_func2;
    delete uctx_main;
    delete uctx_disp;

    delete[] func1_stack;
    delete[] func2_stack;
    delete[] disp_stack;

    std::cout << "The prog has finished\n";
    return 0;
}