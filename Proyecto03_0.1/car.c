#include "car.h"

int ambPerc = 0;
int carCounter = 0;
int mediaE = 3;
int mediaW = 9;
int minSPD = 15;
int maxSPD = 50;
pthread_mutex_t mutexCounter = PTHREAD_MUTEX_INITIALIZER;


//Method definition
car* createCar(int speed, int isAmbulance) {
    car *c;
    c = (car *) malloc(sizeof(car));
    c->id = carCounter;
    c->speed = speed;
    c->direction = 0;
    c->isAmbulance = 0;
    c->corner = 96;
    c->position = 22;
    if(isAmbulance == 0)c->model = uniform(1,10);
    else{ 
        c->isAmbulance = 1;
        c->model = 6;
    }
    c->stops = uniform(1,20);
    ++carCounter;
    return c;
}

void *carCreatorAux(void *param) {
    int amb = (int) param;
	int speed = uniform(minSPD,maxSPD);  
    car *c = createCar(speed, amb);
    struct carNode *node = (struct carNode*)malloc(sizeof(struct carNode));
    node->item = c;
    node->next = NULL;
//    pushCarInList(node);
    carStart(c);
}

void *carCreator(void* isAmbulance) {
    printf("%s\n", "Creating car!");
    pthread_t carCreatorAuxThread;
    pthread_create(&carCreatorAuxThread, NULL, carCreatorAux,isAmbulance);        
}
