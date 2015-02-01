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

void finishConsumidor(struct Flags* flags)
{
  decreaseConsumidor(flags);
}

int main(int argc, char* argv[])
{

 if ( argc != 3 )
    {
        printf( "usage: %s <NOMBRE_BUFFER> <MEDIA_SEGUNDOS>", argv[0] );
    }
    else
    {

     struct Flags* flags = getFlags();
     struct Buffer* buffer = getBuffer();
     float media=atof(argv[2]);
     int id = increaseConsumidor(flags);
     printf("ID=[%d]\n", id);
     char* message;
     int index;
     int i = 0;
     while(0 == isFinished(flags))
     {
        index = readMessage(buffer, flags, &message);
        if(0 > index)
           printf("No message to consume yet on the buffer\n");
        else
        {
           printf("Consumidor id=[%d] index=[%d], message=[%s]\n", id, index, message);
           free(message);
        }

    int timeSleep = exponentialRand(media);
    printf("timesleep: %d ",timeSleep);
    sleep(timeSleep*2);

    if(i++ == 5)
      break;
  }
  finishConsumidor(flags);
}
  return 0;
}
