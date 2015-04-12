#include "SDL/SDL.h"
#include "streetsUtils.h"
#include "carUtils.h"

struct streetSpaceInfo* graph;
unsigned short graphLength;

void initialize()
{
    srand(time(0));
    graphLength = 0;
    graph = loadStreetInfo(&graphLength);
    int i;
    for(i = 0 ; i < graphLength ; i++)
    {
        printf("ID=[%d] => Resistance=[%d] Destinies=[%d | %d]\n", i, graph[i].resistance, graph[i].destiny[0], graph[i].destiny[1]);
    }
    
}

void finish()
{
    free(graph);
}

int main()
{
  initialize();
  
  struct vehicleInfo* car = createCar(graphLength, 0);
  printVehicle(car);
  printf("ss\n");
  free(car);
  
  finish();
  return 0;
}