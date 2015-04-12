#ifndef STREETSUTILS_H
#define STREETSUTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>

#define DESTINY_FILE "destiny.csv"
#define RESISTANCE_FILE "resistance.csv"
#define SPACE_IDS_FILE "map.csv"
#define MAX_SPACES 1000
#define MAX_DESTINIES 2
#define MAX_LENGTH_DESTINY_LINE 50


struct vehicleInfo;

struct bridgeInfo
{
    char* name;
    struct vehicleInfo* vehicles[6];
};
struct streetSpaceInfo
{
    struct vehicleInfo* vehicle;
    short destiny[MAX_DESTINIES];
    struct bridgeInfo* bridge;
    unsigned short maxSpeed;
    unsigned short busStop;
    unsigned short underMaintenance;
    unsigned short resistance;
};


struct streetSpaceInfo* loadStreetInfo(unsigned short* spaceLength);

#endif