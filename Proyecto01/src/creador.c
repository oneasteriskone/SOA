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
  //int semId = createSemaphore();
  createFlags();
  createBuffer();
  createFinalFlag();
    
  return 0;
}
