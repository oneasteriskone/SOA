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

void main(){
	raiseFinalFlag();
	printf("Terminando los procesos de consumidor y procesos de productor...\n");
}

