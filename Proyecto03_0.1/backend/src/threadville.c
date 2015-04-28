//#include "SDL/SDL.h"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#include "streetsUtils.h"
#include "vehicleUtils.h"


#define CHILDCOUNT 15

pthread_t childs[CHILDCOUNT];
//int countChilds;

struct StreetSpaceInfo* graph;
unsigned short graphLength;

void initialize()
{
    srand(time(0));
    graphLength = 0;
    graph = loadStreetInfo(&graphLength);
    int i;
    for(i = 0 ; i < CHILDCOUNT ; i++)
        childs[i] = 0;
    //countChilds = 0
}

int getChildId()
{
    static int countChilds = 0;
    return countChilds++;
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
    
    int i;
    for(i = 0; i < CHILDCOUNT; i++)
    {
        struct ThreadAttributes* attributes = initThreadAttributes();
        attributes->vehicle = createVehicle(graphLength, UNDEF);
        int id = getChildId();
        attributes->vehicle->id = id;
        attributes->id = id;
        printVehicle(attributes->vehicle);
        pthread_t child;
        pthread_create(&child, 0, runVehicleThread, (void*) attributes);
        printf("Created id=[%d] pthread=[%d]\n", id, (int)child);
        childs[id] = child;
        usleep(500 * 1000);
    }
    for(i = 0; i < CHILDCOUNT; i++)
    {
        pthread_join(childs[i], 0);
        printf("==== Joined=[%d|%d] ====\n", i, (int)childs[i]);
    }
    printf("ss\n");
    finish();
    return 0;
}
