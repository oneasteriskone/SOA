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
	raiseEndFlag();
	printf("Terminando los N procesos de consumidor y N procesos de productor...");
}

