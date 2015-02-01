#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "utils.h"
#include "mmapUtils.h"
#include "semsUtils.h"
#include "flagsUtils.h"
#include "bufferUtils.h"
#include "consumidorUtils.h"
#include "finalizadorUtils.h" 

void finishConsumidor(struct Flags* flags)
{
  decreaseConsumidor(flags);
}

int main(int argc, char* argv[])
{
  struct Flags* flags = getFlags();
  struct Buffer* buffer = getBuffer();

  int id = increaseConsumidor(flags);
  printf("ID=[%d]\n", id);
  char* message;
  int index;
  int i = 0;
  while(0 == isFinished(flags))
  {

    printf("Final flag: %d\n", getFinalFlag());
    if(getFinalFlag() == 1){
      finishConsumidor(flags);
      break;
    }

    index = readMessage(buffer, flags, &message);
    if(0 > index){
        printf("No message to consume yet on the buffer\n");
    }else{
      printf("Consumidor id=[%d] index=[%d], message=[%s]\n", id, index, message);
      free(message);
    }
    int timeSleep = exponentialRand(MEDIA);
    sleep(timeSleep*2);

    // if(i++ == 5)
    //   break;
  }
  finishConsumidor(flags);
  return 0;
}
