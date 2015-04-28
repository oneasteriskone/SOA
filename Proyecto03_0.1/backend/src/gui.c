#include "gui.h"

#include <stdio.h>
#include <stdlib.h>

void moveVehicle(struct StreetSpaceInfo* space, struct VehicleInfo* vehicle)
{
    printf("Moving vehicle=[%d] to %d\n", vehicle->id, vehicle->currentSpace);
}