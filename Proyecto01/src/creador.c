#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include "mmapUtils.h"
#include "semsUtils.h"
#include "flagsUtils.h"
#include "bufferUtils.h"
#include "finalizadorUtils.h"

int main(int argc, char* argv[])
{
    if ( argc != 4 ) 
    {
        printf( "usage: %s <BUFFER_SIZE> <PRODUCER_COUNT> <CONSUMER_COUNT> \n", argv[0] );
    }
    else 
    {

        //int semId = createSemaphore();
        FILE *f;
        f = fopen("src/creadorVariables.h", "w");
        int Buff_size;
        Buff_size=atoi(argv[1]);
        int prod;
        prod=atoi(argv[2]);
        int cons;
        cons=atoi(argv[3]);
        fprintf(f, "#define BUFFER_SIZE %d\n#define PRODUCERS_COUNT %d\n#define CONSUMERS_COUNT %d", Buff_size, prod, cons);
        fclose(f);
        createFlags();
        createBuffer();
  		createFinalFlag();
    }
  	return 0;
}
