
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include "semsUtils.h"

#define BUFFER_SEM_KEY 0xA61531

void runChild(int childIndex)
{
  sleep(2);
  int semId = getSemaphore(BUFFER_SEM_KEY);
  printf("Sleeping child %d for %d seconds\n", childIndex, childIndex * 2);
  sleep(childIndex * 2);
  printf("Releasing %d child\n", childIndex);
  signalSemaphore(semId);
  exit(0);
}

int main(int argc, char* argv[])
{
  for (int i = 0 ; i < 5 ; i ++)
    if(0 == fork())
      runChild(i);
  int semId = createSemaphore(BUFFER_SEM_KEY);
  waitSemaphore(semId);
  for (int i = 0 ; i < 5 ; i ++)
  {
    waitSemaphore(semId);
    printf("Released %d child\n", i);
  }
}
