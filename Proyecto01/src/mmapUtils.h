#ifndef MMAP_UTILS
#define MMAP_UTILS

#include <err.h>
#include <stdarg.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <fcntl.h>

void* createMmap(char* path, int size)
{
  int handler = open(path, O_RDWR | O_CREAT | O_TRUNC, 0777);
  if(handler < 0)
    err(EXIT_FAILURE, "Error creating handler for the file=[%s].", path);
  int result = ftruncate(handler, size);
  if (result < 0) {
    close(handler);
    err(EXIT_FAILURE, "Error the file=[%s]", path);
  }

  void* address = (void*) mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED, handler, 0);
  if(address == MAP_FAILED)
    err(EXIT_FAILURE, "Can't create the shared shared memory on file=[%s].", path);

  return address;
}

void* getMmap(char* path, int size)
{
  int handler = open(path, O_RDWR, 0777);
  if(handler < 0)
    err(EXIT_FAILURE, "Error creating handler for the file=[%s]. Has the creador run?", path);
  void* address = (void*) mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED, handler, 0);
  if(address == MAP_FAILED)
    err(EXIT_FAILURE, "Can't create the shared shared memory on file=[%s]. Has the creador run?", path);

  return address;
}

#endif
