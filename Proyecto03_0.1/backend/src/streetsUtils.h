#ifndef STREETSUTILS_H
#define STREETSUTILS_H

#include "threadvilleDefinitions.h"


struct StreetSpaceInfo* loadStreetInfo(unsigned short* spaceLength);
short* findShortestRoute(struct StreetSpaceInfo* graph, unsigned short graphLength, int source, int destiny);
int getSpace(struct StreetSpaceInfo* space, struct VehicleInfo* vehicle);
void releaseSpace(struct StreetSpaceInfo* space);

void printStreets(struct StreetSpaceInfo* graph, int graphLength);
void printRoute(short* route);

#endif
