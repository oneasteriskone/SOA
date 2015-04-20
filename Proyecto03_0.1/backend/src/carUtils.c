#include "carUtils.h"

//incluye o no el max??
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


//public

struct vehicleInfo* createCar(unsigned short top, unsigned short color)
{
    struct vehicleInfo* car = (struct vehicleInfo*) calloc(1, sizeof(struct vehicleInfo));
    if(0 == color)
        car->color = genRandomCarColor();
    else
        car->color = color;
    car->type = 1;
    car->currentx = 0;
    car->currenty = 0;
    car->travels = fillRoute(&(car->route), top);
    return car;
}

void printVehicle(struct vehicleInfo* vehicle)
{
    printf("Type=[%d]\n", vehicle->type);
    printf("Color=[%d]\n", vehicle->color);
    printf("Travels=[%d]\n", vehicle->travels);
    printf("CurrentX=[%d]\n", vehicle->currentx);
    printf("CurrentY=[%d]\n", vehicle->currenty);
    printf("Type=[%d], Color=[%d], Travels=[%d]\n", vehicle->type, vehicle->color, vehicle->travels);
    int i;
    for(i = 0; i < vehicle->travels ; i++)
       printf("\tRoute=[%d,%d]\n", i, vehicle->route[i]);
    printf("\n");
}


//TESTING
//TODO: This functions need to be moved.

void *carCreator(void* isAmbulance) {    
    printf("%s\n", "Creating car!");
    //pthread_t carCreatorAuxThread;
    //pthread_create(&carCreatorAuxThread, NULL, carCreatorAux,isAmbulance);   

    //Include the create car and the addCarToList in a single method.

}
        

void addCarToCarList(struct vehicleInfo* car, struct vehicleInfo* carList, int amountOfCars){
    printf("Amount %d\n", amountOfCars);
    printVehicle(car);

    carList[amountOfCars] = *car;    
}

///////