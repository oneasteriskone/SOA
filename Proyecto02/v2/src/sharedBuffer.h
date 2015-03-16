#ifndef MMAP_UTILS
#define MMAP_UTILS

#include <err.h>
#include <stdarg.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>



struct Buffer
{
  void* address;
  int semId;
  int entries;
  int typeSize;
  char* path;
};

union semun {
  int val;
  struct semid_ds *buf;
  unsigned short int *array;
  struct seminfo *__buf;
};

void sendKillToChilds(struct Buffer* buffer);
int mustDie(struct Buffer* buffer);
int pushValueInBuffer(struct Buffer* buffer, int pid);
void removeValueFromBuffer(struct Buffer* buffer, int pid);
int popValueFromBuffer(struct Buffer* buffer);
int* getValuesFromBuffer(struct Buffer* buffer, int release);

struct Buffer* initBuffer(int size, char* path, int semKey);
struct Buffer* createBuffer(char* path, int size, int semKey);
void destroyBuffer(struct Buffer* buffer);

int createSemaphore(int key, int initValue);
void destroySemaphore(int semId);
void waitSemaphore(int semId);
void signalSemaphore(int semId);

void* createMmap(char* path, int size);
void* getMmap(char* path, int size);
void destroyMmap(char* address, int size);

#endif
