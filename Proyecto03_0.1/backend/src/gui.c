#include "gui.h"

#include <stdio.h>
#include <stdlib.h>

void moveVehicle(struct StreetSpaceInfo* space, struct VehicleInfo* vehicle)
{
    printf("Moving vehicle to %d with resistance=[%d]\n", vehicle->currentSpace, space->resistance);
}