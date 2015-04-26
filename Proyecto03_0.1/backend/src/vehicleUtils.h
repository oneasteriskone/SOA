#ifndef CARUTILS_H
#define CARUTILS_H

#include "threadvilleDefinitions.h"

struct VehicleInfo* createCar(unsigned short top, unsigned short color);
void runVehicleThread(struct ThreadAttributes* attributes);

//debug helpers
void printVehicle(struct VehicleInfo* vehicle);

#endif
