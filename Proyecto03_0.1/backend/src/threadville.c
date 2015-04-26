//#include "SDL/SDL.h"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "streetsUtils.h"
#include "vehicleUtils.h"


struct StreetSpaceInfo* graph;
unsigned short graphLength;

void initialize()
{
    srand(time(0));
    graphLength = 0;
    graph = loadStreetInfo(&graphLength);
}

void finish()
{
    free(graph);
}
struct ThreadAttributes* initThreadAttributes()
{
    struct ThreadAttributes* attributes = (struct ThreadAttributes*)calloc(1, sizeof(struct ThreadAttributes));
    attributes->graphLength = graphLength;
    attributes->graph = graph;
    return attributes;
}
int main()
{
    initialize();
    
    struct ThreadAttributes* attributes = initThreadAttributes();
    attributes->vehicle = createCar(graphLength, 0);
    printVehicle(attributes->vehicle);
    pthread_t child;
    pthread_create(&child, 0, runVehicleThread, (void*) attributes);
    printf("ss\n");
    
    finish();
    return 0;
}
