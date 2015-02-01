#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "utils.h"
#include "mmapUtils.h"
#include "semsUtils.h"
#include "flagsUtils.h"
#include "bufferUtils.h"
#include "productorUtils.h"
#include "finalizadorUtils.h"

void finishProductor(struct Flags* flags)
{
  decreaseProductor(flags);
}

char* buildMessage(int id)
{
  const int bufferRowSize = getBufferRowSize();
  char* message = (char*) calloc(bufferRowSize, sizeof(char));

  time_t rawTime;
  struct tm * timeStruct;
  time(&rawTime);
  timeStruct = localtime(&rawTime);

  int length = snprintf(message, bufferRowSize, "%03d|%04d/%02d/%02d %02d:%02d:%02d|%1d", id, timeStruct->tm_year + 1900, timeStruct->tm_mon, timeStruct->tm_mday, timeStruct->tm_hour, timeStruct->tm_min, timeStruct->tm_sec, rand() % 5);
  return message;
}

int main(int argc, char* argv[])
{  
  if ( argc != 3 )
  {
    printf( "usage: %s <NOMBRE_BUFFER> <MEDIA_SEGUNDOS>\n", argv[0] );
  }
  else
  {
    float media=atof(argv[2]);
    struct Flags* flags = getFlags();
    struct Buffer* buffer = getBuffer();
    char* name=argv[1];
    int timeAcum=0;
    int id = increaseProductor(flags);
    printf("ID=[%d]\n", id);
    char* message;
    int index;
    int i = 0;
    while(0 == isFinished(flags)){
      if(getFinalFlag() == 1){
        finishProductor(flags);
        printf("\nEstadisticas de productor id: %d\n**********************************\nNombre de buffer: %s\nTiempo Consumido: %d\nNumero de mensajes producidos: %d\n**********************************\n",id,name,timeAcum,i);
        break;
      }
      message = buildMessage(id);
      index = writeMessage(buffer, flags, message);
      printf("Wrote in index=[%d] the message=[%s]\n", index, message);
      free(message);
      int timeSleep = exponentialRand(media);
      sleep(timeSleep);
      timeAcum+=timeSleep;
      if(index != -1)
          i++;
      // if(i++ == 10)
      //     break;
    }
    finishProductor(flags);
  }

  return 0;
}
