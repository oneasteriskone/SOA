#ifndef FLAG_UTILS
#define FLAG_UTILS
#define KILLEM_AL_INDEX 0
#define PRODUCERS_INDEX 1
//#define PRODUCERS_COUNT 2
#define CONSUMERS_INDEX 3
//#define CONSUMERS_COUNT 4
#include "creadorVariables.h"
#define CURRENT_INDEX 5
#define BUFFER_WRITE_INDEX 6
#define BUFFER_READ_INDEX 7
#define FLAG_COUNTER 8
#define FLAGS_FILE "/soa_flags"
#define FLAGS_FILE_SIZE FLAG_COUNTER * sizeof(int)

#define FLAGS_SEM_KEY 0XA61532
 
struct Flags
{
  int* address;
  int semId;
};

void createFlags()
{
  int* flags = (int*) createMmap(FLAGS_FILE, FLAGS_FILE_SIZE);
  for (int i = 0 ; i < FLAG_COUNTER ; i++)
    flags[i] = 0;

  int semId = createSemaphore(FLAGS_SEM_KEY);
}

struct Flags* getFlags()
{
  struct Flags* flags = (struct Flags*) calloc(1, sizeof(struct Flags));
  flags->address = (int*) getMmap(FLAGS_FILE, FLAGS_FILE_SIZE);
  flags->semId = getSemaphore(FLAGS_SEM_KEY);
  return flags;
}

int isFinished(struct Flags* flags)
{
  int isFinish;
  waitSemaphore(flags->semId);
  isFinish = flags->address[KILLEM_AL_INDEX];
  signalSemaphore(flags->semId);
  return isFinish;
}

int increaseSubject(struct Flags* flags, const int index, const int counter)
{
  waitSemaphore(flags->semId);
  int id = flags->address[index]++;
  flags->address[counter]++;
  signalSemaphore(flags->semId);
  return id;
}

void decreaseSubject(struct Flags* flags, const int counter)
{
  waitSemaphore(flags->semId);
  flags->address[counter]--;
  signalSemaphore(flags->semId);
}
#endif
