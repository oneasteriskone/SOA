#include <stdio.h>
#include <stdlib.h>
#define MAX_LENGTH 100

void configReader(int *mediaE, int *mediaW, int *minSPD, int *maxSPD, int *ambPerc,
int *carCounter, int *policeCarsPerTurnE, int *policeCarsPerTurnW, int *policeCarCounterW, int *policeCarCounterE, int *bridgeSize, int *kSemE, int *kSemW, int *semGreenE, int *semGreenW, int *strategy, int *bridgeIsFreeE, int *bridgeIsFreeW, int *isFirstInLineAmbE, int *isFirstInLineAmbW, int *maxSpeedInBridge, int *lastCarInBridge) {

    char line[MAX_LENGTH];
    FILE *fp = fopen("config.txt", "r");

    while (fgets(line, MAX_LENGTH, fp)) {
		//printf("line = %s", line);
        sscanf(line, "mediaE %d", mediaE);
		//printf("carMean after sscanf = %d\n",  *carMean);
        sscanf(line, "mediaW %d", mediaW);
		//printf("carCounter after sscanf = %d\n",  *carCounter);
		sscanf(line, "minSPD %d", minSPD);
		sscanf(line, "maxSPD %d", maxSPD);
		sscanf(line, "ambPerc %d", ambPerc);
		sscanf(line, "carCounter %d", carCounter);
		sscanf(line, "policeCarsPerTurnE %d", policeCarsPerTurnE);
		sscanf(line, "policeCarsPerTurnW %d", policeCarsPerTurnW);
		sscanf(line, "policeCarCounterW %d", policeCarCounterW);
		sscanf(line, "policeCarCounterE %d", policeCarCounterE);
		sscanf(line, "bridgeSize %d", bridgeSize);
		sscanf(line, "kSemE %d", kSemE);
		sscanf(line, "kSemW %d", kSemW);
		sscanf(line, "semGreenE %d", semGreenE);
		sscanf(line, "semGreenW %d", semGreenW);
		sscanf(line, "strategy %d", strategy);
		sscanf(line, "bridgeIsFreeE %d", bridgeIsFreeE);
		sscanf(line, "bridgeIsFreeW %d", bridgeIsFreeW);
		sscanf(line, "isFirstInLineAmbE %d", isFirstInLineAmbE);
		sscanf(line, "isFirstInLineAmbW %d", isFirstInLineAmbW);
		sscanf(line, "maxSpeedInBridge %d", maxSpeedInBridge);
		sscanf(line, "lastCarInBridge %d", lastCarInBridge);

    }
    fclose(fp);
}
