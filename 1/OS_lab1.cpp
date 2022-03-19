#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

void* game1(void* flag)
{
  printf("\x1b[31mThread 1 have started\x1b[0m\n");
  int i = 1;
  sleep(2);
  while(*((int*)flag) != 0)
  {
    printf("\x1b[32m\nI'm still waiting, it's been %d seconds! Do your stuff faster", 5*i);
    sleep(2);
    printf("\x1b[0m");
    i++;
  }

  printf("\x1b[31mThread 1 have been finished\x1b[0m\n");
  pthread_exit(NULL);
}

void* game2(void* flag)
{
  printf("\x1b[31mThread 2 have started\x1b[0m\n");
  int *res = new int;
  *res = 0;
  printf("Enter a positive number, please\n");
  scanf("%d", res);
  if(*res > 0)
  {
    *res = 1;
  }
  else if (*res < 0)
  {
    *res = -1;
  } 
  *((int*)flag) = 0;
  printf("\x1b[31m\nThread 2 have been finished\x1b[0m\n");
  pthread_exit((void*)res);
}

int main()
{
  pthread_t first, second;
  int* flag = new int;
  *flag = 1;
  pthread_create(&first, NULL, &game1, ((void*)flag));
  pthread_create(&second, NULL, &game2, ((void*)flag));

  int* out;
  pthread_join(second, (void**)&out);
  pthread_join(first, NULL);
  if(*out > 0)
  {
    printf("Your number is correct\n");
  }
  else if(*out < 0)
  {
    printf("Error! Your number is negative\n");
  }
  else 
  {
    printf("You entered zero!\n");
  }
  printf("The program have been f\n");

  delete out;
  delete flag;
  return 0;
}

/* Hello from dudypool */