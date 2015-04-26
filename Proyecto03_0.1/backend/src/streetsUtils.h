#ifndef STREETSUTILS_H
#define STREETSUTILS_H

#include "threadvilleDefinitions.h"


struct StreetSpaceInfo* loadStreetInfo(unsigned short* spaceLength);
short* findShortestRoute(struct StreetSpaceInfo* graph, unsigned short graphLength, int source, int destiny);
void getSpace(struct StreetSpaceInfo* space, struct VehicleInfo* vehicle);
void releaseSpace(struct StreetSpaceInfo* space, struct VehicleInfo* vehicle);

void printStreets(struct StreetSpaceInfo* graph, int graphLength);
void printRoute(short* route);

#endif
