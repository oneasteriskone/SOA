#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <math.h>

extern int ambPerc;
extern int carCounter;
extern int mediaE;
extern int mediaW;
extern int minSPD;
extern int maxSPD;


typedef struct{
    int id;
    int speed;
    int corner;
    int position;
    int direction; // E == 0 W == 1 N==2 S==3
    int model;
    int stops;
    int route[25];
    int step;
    int isAmbulance;//yes = 1 no = 0
}car;

struct carNode{
     car* item;
     struct carNode *next;
};

typedef struct{
     struct carNode *head;
     struct carNode *last;
    int size;
}carList;

car *createCar(int,int);
void *carCreatorAux(void*);
void *carCreator(void*);
