#ifndef BUFFER_UTILS
#define BUFFER_UTILS
#include <string.h>
#include "creadorVariables.h"
//#define BUFFER_SIZE 150
#define BUFFER_ROW_SIZE 50
#define BUFFER_FILE "/soa_buffer"
#define BUFFER_FILE_SIZE BUFFER_ROW_SIZE * BUFFER_SIZE * sizeof(char)

#define BUFFER_SEM_KEY 0xA61531

struct Buffer
{
  char* address;
  int semId;
};


void createBuffer()
{
  char* buffer = (char*) createMmap(BUFFER_FILE, BUFFER_FILE_SIZE);
  char* current = buffer;
  for(int i = 0 ; i < BUFFER_SIZE ; i++)
  {
    current = buffer + BUFFER_ROW_SIZE * i;
    for(int j = 0 ; j < BUFFER_ROW_SIZE ; j++)
    {
      current[j] = '\0';
    }
  }
  int semId = createSemaphore(BUFFER_SEM_KEY);
}

struct Buffer* getBuffer()
{
  struct Buffer* buffer = (struct Buffer*) calloc(1, sizeof(struct Buffer));
  buffer->address = (char*) getMmap(BUFFER_FILE, BUFFER_FILE_SIZE);
  buffer->semId = getSemaphore(BUFFER_SEM_KEY);
  return buffer;
}

void destroyBuffer()
{
}

const int getBufferRowSize()
{
  return BUFFER_ROW_SIZE;
}
#endif
