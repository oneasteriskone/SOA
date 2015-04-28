#ifndef CARUTILS_H
#define CARUTILS_H

#include "threadvilleDefinitions.h"

struct VehicleInfo* createVehicle(unsigned short top, enum VehicleType type);
void* runVehicleThread(void* attributes);

//debug helpers
void printVehicle(struct VehicleInfo* vehicle);

#endif
