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
unsigned short genRandomCarType()
{
    return genRandomFromRange(RED, BUS_BLUE);
}

unsigned short fillRandomRoute(unsigned short** route, unsigned short top)
{
    unsigned short travels = genRandomTravels();
    *route = (unsigned short*) calloc(travels, sizeof(unsigned short));
    int i;
    for(i = 0; i < travels ; i++)
    {
        (*route)[i] = genRandomFromRange(0, top);
        printf("i=[%d] value=[%d] > top=[%d]\n", i, (*route)[i], top);
    }
    (*route)[travels++] = 507;
    return travels;
}
unsigned short fillAmbulanceRoute(unsigned short** route, unsigned short top)
{
    unsigned short travels = 3;
    *route = (unsigned short*) calloc(travels, sizeof(unsigned short));
    (*route)[0] = genRandomFromRange(0, top);
    (*route)[1] = genRandomFromRange(0, top);
    (*route)[2] = 492;
    return travels;
}
unsigned short fillBusRedRoute(unsigned short** route)
{
    unsigned short travels = 12;
    *route = (unsigned short*) calloc(travels, sizeof(unsigned short));
    (*route)[0] = 26;
    (*route)[1] = 45;
    (*route)[2] = 59;
    (*route)[3] = 500;
    (*route)[4] = 68;
    (*route)[5] = 82;
    (*route)[6] = 106;
    (*route)[7] = 125;
    (*route)[8] = 139;
    (*route)[9] = 516;
    (*route)[10] = 148;
    (*route)[11] = 2;
    return travels;
}
unsigned short fillBusBlueRoute(unsigned short** route)
{
    unsigned short travels = 6;
    *route = (unsigned short*) calloc(travels, sizeof(unsigned short));
    (*route)[0] = 90;
    (*route)[1] = 117;
    (*route)[2] = 136;
    (*route)[3] = 514;
    (*route)[4] = 497;
    (*route)[5] = 71;
    return travels;
}
unsigned short fillBusGreenRoute(unsigned short** route)
{
    unsigned short travels = 6;
    *route = (unsigned short*) calloc(travels, sizeof(unsigned short));
    (*route)[0] = 56;
    (*route)[1] = 504;
    (*route)[2] = 520;
    (*route)[3] = 151;
    (*route)[4] = 10;
    (*route)[5] = 37;
    return travels;
}

unsigned short fillRoute(enum VehicleType type, unsigned short** route, unsigned short top)
{
    switch(type)
    {
        case BUS_ORANGE:
            return fillRandomRoute(route, top);
        case BUS_PINK:
        case BUS_BLUELIGHT:
            return fillRandomRoute(route, top);
        case BUS_WHITE:
        case BUS_BLACK:
        case BUS_GRAY:
            return fillRandomRoute(route, top);
        case BUS_RED:
            return fillBusRedRoute(route);
        case BUS_GREEN:
            return fillBusGreenRoute(route);
        case BUS_BLUE:
            return fillBusBlueRoute(route);
        case AMBULANCE:
            return fillAmbulanceRoute(route, top);
        default:
            return fillRandomRoute(route, top);
    }
}
unsigned short initSpace(enum VehicleType type)
{
    switch(type)
    {
        case BUS_ORANGE:
            return 177;
        case BUS_PINK:
            return 421;
        case BUS_BLUELIGHT:
            return 125;
        case BUS_WHITE:
            return 275;
        case BUS_BLACK:
            return 307;
        case BUS_GRAY:
            return 106;
        case BUS_RED:
            return 2;
        case BUS_GREEN:
            return 37;
        case BUS_BLUE:
            return 71;
        case AMBULANCE:
            return 508;
        default:
            return 508;
    }
}

void runVehicle(struct VehicleInfo* vehicle, struct StreetSpaceInfo* graph, unsigned short graphLength, short* route)
{
    int i;
    for(i = 1; i <= route[0]; i++)
    {
        if(graphLength <= route[i])
            err(1, "aVehicle=[%d] length=[%d] space=[%d]", vehicle->id, route[0], route[i]);
        while(0 == getSpace(&(graph[route[i]]), vehicle))
        {
            //printf("spleeping=[%d] for [%d]\n", vehicle->id, route[i]);
            usleep(vehicle->speed);
        }
        if(graphLength <= vehicle->currentSpace)
            err(1, "bVehicle=[%d] length=[%d] space=[%d]", vehicle->id, route[0], vehicle->currentSpace);
        releaseSpace(&(graph[vehicle->currentSpace]));
        
        moveVehicle(&(graph[route[i]]), vehicle);
        
        vehicle->currentSpace = route[i];
        usleep(vehicle->speed);
    }
}

void freeVehicle(struct VehicleInfo* vehicle, struct StreetSpaceInfo* graph)
{
    releaseSpace(&(graph[vehicle->currentSpace]));
    free(vehicle);
}

void printStatistics(struct VehicleInfo* vehicle)
{
}

int getStopTime(enum VehicleType type)
{
    if(type > AMBULANCE)
        return 5000 * 1000;
    else if(type == AMBULANCE)
        return 8000 * 1000;
    else
        return 0;
}

int getVehicleSpeed(enum VehicleType type)
{
    switch(type)
    {
        case BLUE:
            return 2*1000;
        case GREEN:
            return 3*1000;
        case BLACK:
            return 4*1000;
        case WHITE:
            return 5*1000;
        case YELLOW:
            return 6*1000;
        case AMBULANCE:
            return 0.5*1000;
        case BUS_ORANGE:
            return 7*1000;
        case BUS_PINK:
        case BUS_BLUELIGHT:
            return 3*1000;
        case BUS_WHITE:
        case BUS_BLACK:
        case BUS_GRAY:
            return 4*1000;
        case BUS_RED:
        case BUS_GREEN:
        case BUS_BLUE:
            return 5*1000;
        default:
            return 1*1000;
    }
}

//public
struct VehicleInfo* createVehicle(unsigned short top, enum VehicleType type)
{
    struct VehicleInfo* vehicle = (struct VehicleInfo*) calloc(1, sizeof(struct VehicleInfo));
    if(UNDEF == type)
        vehicle->type = genRandomCarType();
    else
        vehicle->type = type;
    vehicle->travels = fillRoute(vehicle->type, &(vehicle->route), top);
    vehicle->currentSpace = initSpace(vehicle->type);
    vehicle->speed = (getVehicleSpeed(vehicle->type) * 1000)/44;
    vehicle->stopTime = getStopTime(vehicle->type);
    return vehicle;
}

void* runVehicleThread(void* tmpAttributes)
{
    struct ThreadAttributes* attributes = (struct ThreadAttributes*) tmpAttributes;
    int i;
    for(i = 0 ; i < attributes->vehicle->travels ; i++)
    {
        // printf("id=[%d] i=[%d] space=[%d]\n", attributes->id, i, attributes->vehicle->route[i]);
        short* route = findShortestRoute(attributes->graph, attributes->graphLength, attributes->vehicle->currentSpace, attributes->vehicle->route[i]);
        runVehicle(attributes->vehicle, attributes->graph, attributes->graphLength, route);
        free(route);
        if(i < attributes->vehicle->travels - 1)
            usleep(attributes->vehicle->stopTime);
    }
    printStatistics(attributes->vehicle);
    freeVehicle(attributes->vehicle, attributes->graph);
    attributes->vehicle = 0;
    attributes->graph = 0;
    free(attributes);
    return 0;
}

void printVehicle(struct VehicleInfo* vehicle)
{
    printf("ID=[%d] Type=[%d], StopTime=[%d] Speed=[%d] Travels=[%d]\n", vehicle->id, vehicle->type, vehicle->stopTime, vehicle->speed, vehicle->travels);
    int i;
    for(i = 0; i < vehicle->travels ; i++)
        printf("\tRoute=[%d,%d]\n", i, vehicle->route[i]);
    printf("\n");
}

