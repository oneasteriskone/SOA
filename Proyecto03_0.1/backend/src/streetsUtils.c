#include "streetsUtils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>
#include <limits.h>
#include <unistd.h>

void loadDestiny(struct StreetSpaceInfo* graph, short* ids, const unsigned short spaceLength)
{
    FILE* handler = fopen(DESTINY_FILE, "r");
    if(0 == handler)
        err(1, "The destiny file=[%s] couldn't be loaded", DESTINY_FILE);
    
    char line[MAX_LENGTH_DESTINY_LINE];
    char* split = 0;
    int count = 0;
    int id;
    unsigned short i;
    while(0 != fgets(line, MAX_LENGTH_DESTINY_LINE, handler) && count < spaceLength)
    {
        id = ids[count++];
        split = strtok(line,"|");
        i = 0;
        while(i < MAX_DESTINIES && 0 != split)
        {
            graph[id].destiny[i++] = atoi(split);
            split = strtok(0, "|");
        }
        while(i < MAX_DESTINIES)
            graph[id].destiny[i++] = -1;
    }
    fclose(handler);
}

void loadResistance(struct StreetSpaceInfo* graph, short* ids, unsigned short* spaceLength)
{
    FILE* handler = fopen(RESISTANCE_FILE, "r");
    if(0 == handler)
        err(1, "The destiny file=[%s] couldn't be loaded", RESISTANCE_FILE);
    
    char line[MAX_LENGTH_DESTINY_LINE];
    int count = 0;
    int id;
    while(0 != fgets(line, MAX_LENGTH_DESTINY_LINE, handler) && count < *spaceLength)
    {
        id = ids[count++];
        graph[id].resistance = atoi(line);
    }
    fclose(handler);
}

short* loadSpaceIds(unsigned short* totalSpaceLoaded)
{
    *totalSpaceLoaded = 0;
    FILE* handler = fopen(SPACE_IDS_FILE, "r");
    if(0 == handler)
        err(1, "The destiny file=[%s] couldn't be loaded", SPACE_IDS_FILE);
    
    short* ids = (short*)calloc(MAX_SPACES, sizeof(short));
    char line[MAX_LENGTH_DESTINY_LINE];
    while(0 != fgets(line, MAX_LENGTH_DESTINY_LINE, handler))
    {
        short id = atoi(line);
        ids[(*totalSpaceLoaded)++] = id;
        //printf("id=%d | i=%d\n", id, *totalSpaceLoaded);
    }
    fclose(handler);
    return ids;
}

struct StreetSpaceInfo* loadStreetInfo(unsigned short* spaceLength)
{
    *spaceLength = 0;
    short* ids = loadSpaceIds(spaceLength);
    if(*spaceLength == 0)
        err(1, "No data was loaded from the ids configuration file");
    
    struct StreetSpaceInfo* graph = (struct StreetSpaceInfo*) calloc(*spaceLength, sizeof(struct StreetSpaceInfo));
    loadDestiny(graph, ids, *spaceLength);
    loadResistance(graph, ids, spaceLength);
    //printStreets(graph, *spaceLength);
    return graph;
}

int minDistance(unsigned short * distances, unsigned short* selected)
{
    unsigned short min = USHRT_MAX;
    int minIndex, i;
    for(i = 0 ; i < MAX_SPACES ; i++)
        if(0 == selected[i] && distances[i] <= min)
        {
            min = distances[i];
            minIndex = i;
        }
    return minIndex;
}

short* findShortestRoute(struct StreetSpaceInfo* graph, unsigned short graphLength, int source, int destiny)
{
    unsigned short distances[MAX_SPACES] = {USHRT_MAX};
    int i;
    for(i = 0 ; i < MAX_SPACES ; i++)
        distances[i] = USHRT_MAX;
    distances[source] = 0;
    unsigned short selected[MAX_SPACES] = {0};
    int parent[MAX_SPACES] = {-1};
    printf("From=[%d] to=[%d]\n", source, destiny);
    while(selected[destiny] == 0)
    {
        int minimum = minDistance(distances, selected);
        //printf("Minimum=[%d]\n", minimum);
        selected[minimum] = 1;
        for(i = 0 ; i < MAX_DESTINIES ; i++)
        {
            short temporalSpaceId = graph[minimum].destiny[i];
            //printf("For entry=[%d] with id=[%d] in minimum=[%d] is distance=[%d=(%d+%d)] < current=[%d]\n", i, temporalSpaceId, minimum, distances[minimum] + graph[temporalSpaceId].resistance, distances[minimum], graph[temporalSpaceId].resistance, distances[temporalSpaceId]);
            if(temporalSpaceId >= 0 && 0 == selected[temporalSpaceId] && distances[minimum] + graph[temporalSpaceId].resistance < distances[temporalSpaceId])
            {
                distances[temporalSpaceId] = distances[minimum] + graph[temporalSpaceId].resistance;
                //printf("\tNew distance=[%d] for id=[%d] with parent=[%d]\n", distances[temporalSpaceId], temporalSpaceId, minimum);
                parent[temporalSpaceId] = minimum;
            }
        }
    }
    int current = destiny;
    int count = 0;
    short* inversedPath = (short*) calloc(graphLength, sizeof(short));
    while(current != -1 && current != source)
    {
        //printf("path for j= %d is %d\n",count,current);
        inversedPath[count++] = current;
        current = parent[current];
    }
    short* path = (short*) calloc(count+1, sizeof(short));
    for(i = 0 ; i < count ; i++)
        path[i] = inversedPath[count - 1 - i];
    path[i] = -1;
    printRoute(path);
    return path;
}

void getSpace(struct StreetSpaceInfo* space, struct VehicleInfo* vehicle)
{
}
void releaseSpace(struct StreetSpaceInfo* space, struct VehicleInfo* vehicle)
{
}


void printStreets(struct StreetSpaceInfo* graph, int graphLength)
{
    int i;
    for(i = 0 ; i < graphLength ; i++)
        printf("ID=[%d] => Resistance=[%d] Destinies=[%d | %d]\n", i, graph[i].resistance, graph[i].destiny[0], graph[i].destiny[1]);
}
void printRoute(short* route)
{
    int i = 0;
    while(route[i] != -1)
        printf("%d => ", route[i++]);
    printf("\n");
}