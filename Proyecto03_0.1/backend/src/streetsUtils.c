#include "streetsUtils.h"

void loadDestiny(struct streetSpaceInfo* graph, short* ids, const unsigned short spaceLength)
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

void loadResistance(struct streetSpaceInfo* graph, short* ids, unsigned short* spaceLength)
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

struct streetSpaceInfo* loadStreetInfo(unsigned short* spaceLength)
{
    *spaceLength = 0;
    short* ids = loadSpaceIds(spaceLength);
    if(*spaceLength == 0)
        err(1, "No data was loaded from the ids configuration file");
    
    struct streetSpaceInfo* graph = (struct streetSpaceInfo*) calloc(*spaceLength, sizeof(struct streetSpaceInfo));
    loadDestiny(graph, ids, *spaceLength);
    loadResistance(graph, ids, spaceLength);
    return graph;
}

struct streetSpaceInfo getCoordinates(int n){
    // Get the x and y of a box. 
}