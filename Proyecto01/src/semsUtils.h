#ifndef SEMAPHORE_UTILS
#define SEMAPHORE_UTILS


#include <stdio.h>
#include <err.h>
#include <stdarg.h>

union semun {
  int val;                    /* valor para SETVAL */
  struct semid_ds *buf;       /* buffer para IPC_STAT, IPC_SET */
  unsigned short int *array;  /* array para GETALL, SETALL */
  struct seminfo *__buf;      /* buffer para IPC_INFO */
};

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>


int createSemaphore(int key)
{
  union semun valor;
  int semId = semget(key, 1, 0600|IPC_CREAT);
  valor.val = 1;
  int valueReturned = semctl(semId, 0, SETVAL, valor);
  if(valueReturned < 0)
    err(EXIT_FAILURE, "Couldn't set de semaphore with the key=[%d] and id=[%d]\n", key, semId);
  return semId;
}

int getSemaphore(int key)
{
  return semget(key, 1, 0600|IPC_CREAT);
}

void destroySemaphore(int semId)
{
  int valueReturned = semctl(semId, 0, IPC_RMID);
  if(valueReturned < 0)
    err(EXIT_FAILURE, "Couldn't destroy de semaphore with the id=[%d]\n", semId);
}

void waitSemaphore(int semId)
{
  struct sembuf buffer;
  buffer.sem_num = 0;//único semáforo
  buffer.sem_op = -1;//operación wait
  buffer.sem_flg = 0;//espere
  int valueReturned = semop(semId, &buffer, 1);
  if(valueReturned < 0 )
    err(EXIT_FAILURE, "Couldn't get de semaphore with the id=[%d]\n", semId);
}
void signalSemaphore(int semId)
{
  struct sembuf buffer;
  buffer.sem_num = 0;//único semáforo
  buffer.sem_op = 1;//operación signal
  buffer.sem_flg = 0;//espere
  int valueReturned = semop(semId, &buffer, 1);
  if(valueReturned < 0 )
    err(EXIT_FAILURE, "Couldn't release de semaphore with the id=[%d]\n", semId);
}


#endif
