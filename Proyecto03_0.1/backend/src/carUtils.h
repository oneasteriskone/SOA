#ifndef CARUTILS_H
#define CARUTILS_H

#include <time.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_TRAVELS 10
#define MIN_TRAVELS 3

#define MAX_CARS 100

struct vehicleInfo
{
    unsigned short type;
    unsigned short color;
    unsigned short travels;
    unsigned short currentx;
    unsigned short currenty;
    unsigned short* route;
};

struct vehicleInfo* createCar(unsigned short top, unsigned short color);

//debug helpers
void printVehicle(struct vehicleInfo* vehicle);

#endif