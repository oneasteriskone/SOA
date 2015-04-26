#include "vehicleUtils.h"
#include "streetsUtils.h"
#include "gui.h"

#include <unistd.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include <unistd.h>

unsigned short genRandomFromRange(const unsigned short min, const unsigned short count)
{
    return (rand() % count) + min;
}

unsigned short genRandomTravels()
{
    return genRandomFromRange(MIN_TRAVELS, MAX_TRAVELS - MIN_TRAVELS + 1);
}
unsigned short genRandomCarColor()
{
    return genRandomFromRange(1, 3);
}

unsigned short fillRoute(unsigned short** route, unsigned short top)
{
    unsigned short travels = genRandomTravels();
    *route = (unsigned short*) calloc(travels, sizeof(unsigned short));
    int i;
    for(i = 0; i < travels ; i++)
        (*route)[i] = genRandomFromRange(0, top);
    return travels;
}

void runVehicle(struct VehicleInfo* vehicle, struct StreetSpaceInfo* graph, unsigned short graphLength, short* route)
{
    int i = 0;
    while(-1 != route[i])
    {
        getSpace(&(graph[route[i]]), vehicle);
        releaseSpace(&(graph[vehicle->currentSpace]), vehicle);
        
        moveVehicle(&(graph[route[i]]), vehicle);
        
        vehicle->currentSpace = route[i++];
        usleep(vehicle->sleepTime);
    }
}

void printStatistics(struct VehicleInfo* vehicle)
{
}


//public

struct VehicleInfo* createCar(unsigned short top, unsigned short color)
{
    struct VehicleInfo* vehicle = (struct VehicleInfo*) calloc(1, sizeof(struct VehicleInfo));
    if(0 == color)
        vehicle->color = genRandomCarColor();
    else
        vehicle->color = color;
    vehicle->type = 1;
    vehicle->travels = fillRoute(&(vehicle->route), top);
    vehicle->currentSpace = 492;
    vehicle->sleepTime = 10 * 1000;
    return vehicle;
}

void runVehicleThread(struct ThreadAttributes* attributes)
{
    int i;
    for(i = 0 ; i < attributes->vehicle->travels ; i++)
    {
        short* route = findShortestRoute(attributes->graph, attributes->graphLength, attributes->vehicle->currentSpace, attributes->vehicle->route[i]);
        runVehicle(attributes->vehicle, attributes->graph, attributes->graphLength, route);
        free(route);
    }
    printStatistics(attributes->vehicle);
    free(attributes->vehicle);
    free(attributes);
}

void printVehicle(struct VehicleInfo* vehicle)
{
    printf("Type=[%d], Color=[%d], Travels=[%d]\n", vehicle->type, vehicle->color, vehicle->travels);
    int i;
    for(i = 0; i < vehicle->travels ; i++)
        printf("\tRoute=[%d,%d]\n", i, vehicle->route[i]);
    printf("\n");
}

