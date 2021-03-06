#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <math.h>
#include "car.h"
#include "configReader.h"
#include "dijsktra.h"
#include "readWeightMatrix.h"
#include "usableSpaces.h"
#include "SDL/SDL.h"
#include "readGUIMapping.h"

int policeCarsPerTurnE = 5;
int policeCarsPerTurnW = 7;
int policeCarCounterW = 0;
int policeCarCounterE = 0;
int bridgeSize = 25;
int kSemE = 10;
int kSemW = 12;
int semGreenE = 0;//Yes = 1 No = 0
int semGreenW = 0;//Yes = 1 No = 0
int strategy;
int bridgeIsFreeE = 1; //Yes = 1 No = 0
int bridgeIsFreeW = 1; //Yes = 1 No = 0
int isFirstInLineAmbE = 0;//Yes = 1 No = 0
int isFirstInLineAmbW = 0;//Yes = 1 No = 0
int maxSpeedInBridge = 1000;
int lastCarInBridge = 0;
int letWrite = 1;
int weightMatrix[98][98];
StreetSpace spaceList[1230];//Poderoso array de posiciones
int quit = 0;
int guiMapping [1230][2];
SDL_Surface* threadville = NULL;
SDL_Surface* screen = NULL;
SDL_Surface* carGUI = NULL;
const char *carNames[] = {"redBus","blueBus","greenBus", "yellowBus", "orangeBus", "ambulance", "redCar", "blueCar", "greenCar", "whiteCar"};

carList carListE;
carList carListW;

SDL_Event event;

// Struct definition

struct trafficLight{
    int time;
};


void pushCarInList(struct carNode*);
car* popCarFromList(int); 
float exponential(float);
void carInitiator();
int uniform(int,int);
void carStart( car*);
void carMove( car*);
void carMoveNormal( car*);
void carMoveStopLight( car*);
void carMovePolice( car*);
void PrintKeyInfo( SDL_KeyboardEvent *key );
void PrintModifiers( SDLMod mod );

void *stopLightStart(){
    while(1){
        if(semGreenE == 1){
            semGreenE = 0;
            semGreenW = 1;
            printf("StopLight set green to the West and red to the East\n");
            sleep(kSemW);
            
        }else{
            semGreenW = 0;
            semGreenE = 1;
            printf("StopLight set green to the East and red to the West\n");
            sleep(kSemE);
        }
    }
}

void *policeStart(){	
	policeCarCounterW = policeCarsPerTurnW;
	policeCarCounterE = 0;
    while(1){
        if(policeCarCounterW > 0 && carListW.head != NULL){
			//printf("Policeman stops East traffic\n");
			policeCarCounterE = policeCarsPerTurnE;
        }else if (policeCarCounterE > 0 && carListE.head != NULL){
			//printf("Policeman stops West traffic\n");
			policeCarCounterW = policeCarsPerTurnW;
        }
    }
}


void carStart( car* c){
    int i = 0;
    while(1){
        i = 0;  
        for(i;i<25;i++)c->route[i] = -1;
        c->step = 0;
        int destiny = uniform(0,97);
        if(c->stops == 1) destiny = 97;
        dijsktra(weightMatrix,c->position,destiny,c->route);
        carMove(c);
        c->stops--;
        if(c->stops == 0){
            free(c);
            pthread_exit(NULL);            
        }
    }
}
void carMove(car* c){
    int stepsMoved = 0;
    while(c->route[c->step] != -1){
        int sumVal;
        if(c->corner > c->route[c->step]){
            if((c->corner - c->route[c->step]) > 12){
                sumVal = -41; // Significa que va subiendo
                c->direction = 2;
            }
            else{
                 sumVal = -1;//Va hacia la izquierda
                 c->direction = 0;
            }
        }else{
            if((c->route[c->step] - c->corner) > 12){
                sumVal = 41; // Significa que va bajando
                c->direction = 3;
            }
            else{
                 sumVal = 1;//Va hacia la derecha
                 c->direction = 1;
            }
        }
        int stepsToMove = weightMatrix[c->position][c->route[c->step]];
        while(letWrite != 1)
        letWrite = 0;
        if(spaceList[c->position + sumVal].busy == 0){
            spaceList[c->position].busy = 0;
            spaceList[c->position + sumVal].busy = 1;
            stepsMoved++;
            if(stepsToMove - stepsMoved == 0){
                c->position = c->route[c->step];
                c->step++;
            }
        }
        letWrite = 1;
        
        
    }
}

/*
void carMove( car* c){
    if(c->isAmbulance){
        if(c->direction == 0){
            if(bridgeIsFreeW == 1){
                if(bridgeIsFreeE == 1){
                    bridgeIsFreeE = 0;
                    maxSpeedInBridge = c->speed;
                    lastCarInBridge = c->id;
                    popCarFromList(0);
                    sleep(bridgeSize/c->speed);
                    if(lastCarInBridge == c->id)bridgeIsFreeE = 1;
                }
                else{
                    lastCarInBridge = c->id;
                    popCarFromList(0);
                    if(c->speed > maxSpeedInBridge)sleep(bridgeSize/maxSpeedInBridge);
                    else sleep(bridgeSize/c->speed);
                    if(lastCarInBridge == c->id)bridgeIsFreeE = 1;
                }                
                printf("Ambulance finished crossing the bridge from the East\n");
                free(c);
                pthread_exit(NULL);
            }
        }else{
            if(bridgeIsFreeE == 1){
                if(bridgeIsFreeW == 1){
                    bridgeIsFreeW = 0;
                    maxSpeedInBridge = c->speed;
                    lastCarInBridge = c->id;
                    popCarFromList(1);
                    sleep(bridgeSize/c->speed);
                    if(lastCarInBridge == c->id)bridgeIsFreeW = 1;
                }
                else{
                    lastCarInBridge = c->id;
                    popCarFromList(1);
                    if(c->speed > maxSpeedInBridge)sleep(bridgeSize/maxSpeedInBridge);
                    else sleep(bridgeSize/c->speed);
                    if(lastCarInBridge == c->id)bridgeIsFreeW = 1;
                }
                printf("Ambulance finished crossing the bridge from the West\n");
                free(c);
                pthread_exit(NULL);
            }
        }
    }
    switch(strategy){
        case 1: carMoveNormal(c);//ley de la selva
                break;
        case 2: carMoveStopLight(c);//Semaforo
                break;
        case 3: carMovePolice(c);//Policia
                break;
    }
}*/
/*
void carMoveNormal( car* c){
    if(c->direction == 0){
        if(isFirstInLineAmbW == 0){
            if(bridgeIsFreeW == 1){
                //The car can go inside the bridge
                lastCarInBridge = c->id;
                if(bridgeIsFreeE == 1){
                    maxSpeedInBridge = c->speed;
                    popCarFromList(0);
                    sleep(c->speed);
                }
                else{                    
                    if(c->speed > maxSpeedInBridge){
                        popCarFromList(0);
                        sleep(maxSpeedInBridge);
                    }else{
                        popCarFromList(0);
                        sleep(c->speed);
                    }                      
                }                    
                if(lastCarInBridge == c->id)bridgeIsFreeE = 1;
                printf("Car with id %d finished crossing the bridge from the East\n",c->id);
                free(c);
                pthread_exit(NULL);
            }
        }
    }else{
        if(isFirstInLineAmbE == 0){
            if(bridgeIsFreeE == 1){
                //The car can go inside the bridge
                lastCarInBridge = c->id;
                if(bridgeIsFreeW == 1){
                    maxSpeedInBridge = c->speed;
                    popCarFromList(1);
                    sleep(c->speed);
                }
                else{                    
                    if(c->speed > maxSpeedInBridge){
                        popCarFromList(1);
                        sleep(maxSpeedInBridge);
                    }else{
                        popCarFromList(1);
                        sleep(c->speed);
                    }                      
                }                    
                if(lastCarInBridge == c->id)bridgeIsFreeW = 1;
                printf("Car with id %d finished crossing the bridge from the West\n",c->id);
                free(c);
                pthread_exit(NULL);
            }                
        }
    }
}*/
/*
void carMoveStopLight( car* c){
    if(c->direction == 0){
        if(semGreenE == 1){
            if(isFirstInLineAmbW == 0){
                if(bridgeIsFreeW == 1){
                    //The car can go inside the bridge
                    lastCarInBridge = c->id;
                    if(bridgeIsFreeE == 1){
                        bridgeIsFreeE = 0;
                        maxSpeedInBridge = c->speed;
                        popCarFromList(0);
                        sleep(c->speed);
                    }
                    else{                    
                        if(c->speed > maxSpeedInBridge){
                            popCarFromList(0);
                            sleep(maxSpeedInBridge);
                        }else{
                            popCarFromList(0);
                            sleep(c->speed);
                        }                      
                    }                    
                    if(lastCarInBridge == c->id)bridgeIsFreeE = 1;
                    printf("Car with id %d finished crossing the bridge from the East\n",c->id);
                    free(c);
                    pthread_exit(NULL);
                }                
            }
        }
    }else{
        if(semGreenW == 1){
            if(isFirstInLineAmbE == 0){
                if(bridgeIsFreeE == 1){
                    //The car can go inside the bridge
                    lastCarInBridge = c->id;
                    if(bridgeIsFreeW == 1){
                        bridgeIsFreeW = 0;
                        maxSpeedInBridge = c->speed;
                        popCarFromList(1);
                        sleep(c->speed);
                    }
                    else{                    
                        if(c->speed > maxSpeedInBridge){
                            popCarFromList(1);
                            sleep(maxSpeedInBridge);
                        }else{
                            popCarFromList(1);
                            sleep(c->speed);
                        }                      
                    }                    
                    if(lastCarInBridge == c->id)bridgeIsFreeW = 1;
                    printf("Car with id %d finished crossing the bridge from the West\n",c->id);
                    free(c);
                    pthread_exit(NULL);
                }                
            }
        }
    }
}*/
/*
void carMovePolice( car* c){
   if(c->direction == 0){
        if(policeCarCounterE > 0){
            if(isFirstInLineAmbW == 0){
                if(bridgeIsFreeW == 1){
                    //The car can go inside the bridge
                    lastCarInBridge = c->id;
                    if(bridgeIsFreeE == 1){
                        bridgeIsFreeE = 0;
                        maxSpeedInBridge = c->speed;
                        popCarFromList(0);
						--policeCarCounterE;
                        sleep(c->speed);
                    }
                    else{                    
                        if(c->speed > maxSpeedInBridge){                            
                            popCarFromList(0);
							policeCarCounterE--;
                            sleep(maxSpeedInBridge);
                        }else{
                            popCarFromList(0);
							--policeCarCounterE;
                            sleep(c->speed);
                        }                     
                    }                    
                    if(lastCarInBridge == c->id)bridgeIsFreeE = 1;
                    printf("Car with id %d finished crossing the bridge from the East\n",c->id);
                    free(c);
                    pthread_exit(NULL);
                }                
            }
        }
    }else{
        if(policeCarCounterW > 0){
            if(isFirstInLineAmbE == 0){
                if(bridgeIsFreeE == 1){
                    //The car can go inside the bridge
                    lastCarInBridge = c->id;
                    if(bridgeIsFreeW == 1){
                        bridgeIsFreeW = 0;
                        maxSpeedInBridge = c->speed;
                        popCarFromList(1);
						--policeCarCounterW;
                        sleep(c->speed);
                    }
                    else{                    
                        if(c->speed > maxSpeedInBridge){
                            popCarFromList(1);
							--policeCarCounterW;
                            sleep(maxSpeedInBridge);
                        }else{
                            popCarFromList(1);
							policeCarCounterW--;
                            sleep(c->speed);
                        }                      
                    }                    
                    if(lastCarInBridge == c->id)bridgeIsFreeW = 1;
                    printf("Car with id %d finished crossing the bridge from the West\n",c->id);
                    free(c);
                    pthread_exit(NULL);
                }                
            }
        }
    }
}
*/
int draw(SDL_Surface* Surf_Dest, SDL_Surface* Surf_Src, int X, int Y) {
	if(Surf_Dest == NULL || Surf_Src == NULL) {
		return -1;
	}

	SDL_Rect DestR;

	DestR.x = X;
	DestR.y = Y;

	SDL_BlitSurface(Surf_Src, NULL, Surf_Dest, &DestR);
	SDL_Flip( screen );

	return 0;
}

void drawCars() {

	//while(1){
		int i = 0;
		for (i; i < 1230; i++) {

			if(spaceList[i].usable == 1 && spaceList[i].busy == 1){
				char* carName;

                printf("%s\n", carNames[spaceList[i].model]);

				switch(spaceList[i].direction) {

					case 0:
					carName = strcpy(strcpy("images/", carNames[spaceList[i].model]), "Left.bmp");
					printf("carName value:%s", carName);
					carGUI = SDL_LoadBMP(carName);
					break;

					case 1:
					carName = strcpy(strcpy("images/", carNames[spaceList[i].model]), "Right.bmp");
					printf("carName value:%s", carName);
					carGUI = SDL_LoadBMP(carName);
					break;

					case 2:
					carName = strcpy(strcpy("images/", carNames[spaceList[i].model]), "Up.bmp");
					printf("carName value:%s", carName);
					carGUI = SDL_LoadBMP(carName);
					break;

					case 3:
					carName = strcpy(strcpy("images/", carNames[spaceList[i].model]), "Down.bmp");
					printf("carName value:%s", carName);
					carGUI = SDL_LoadBMP(carName);
					break;
				}
				
				draw(screen, carGUI, guiMapping[i][0],guiMapping[i][1]);
				SDL_Delay(500);
				SDL_Flip(screen);
			}
		}
	//}

}

int main(){
    readWeightMatrix(weightMatrix);
    readUsableFile(spaceList); //llena el poderoso array de posiciones
	readGUIMapping(guiMapping);
	configReader(&mediaE, &mediaW, &minSPD, &maxSPD, &ambPerc, &carCounter, &policeCarsPerTurnE, &policeCarsPerTurnW, &policeCarCounterW, &policeCarCounterE, &bridgeSize, &kSemE, &kSemW, &semGreenE, &semGreenW, &strategy, &bridgeIsFreeE, &bridgeIsFreeW, &isFirstInLineAmbE, &isFirstInLineAmbW, &maxSpeedInBridge, &lastCarInBridge);
    srand(time(NULL));
    ambPerc = uniform(5,15);
	policeCarCounterW = policeCarsPerTurnW;
	policeCarCounterE = policeCarsPerTurnW;
	//char strategySelected;

	pthread_t gui;

    //Start SDL
    SDL_Init(SDL_INIT_EVERYTHING);

    //Set up screen
    screen = SDL_SetVideoMode(888, 925, 24, SDL_SWSURFACE);

	//Enable Unicode translation
    SDL_EnableUNICODE(1);

    //Load image
    //threadville = SDL_LoadBMP("images/threadville.bmp");

    //Apply image to screen
    SDL_BlitSurface(threadville, NULL, screen, NULL);
	SDL_Flip(screen);

	pthread_create(&gui,NULL,drawCars,NULL);

	
    while( !quit ){
        while( SDL_PollEvent(&event) ){
			pthread_t car;
			pthread_t ambulance;
			SDL_BlitSurface(threadville, NULL, screen, NULL);
			SDL_Flip(screen);
            switch(event.type){
                case SDL_KEYUP:
                    switch(event.key.keysym.sym){
						//random car creation
						case SDLK_F1:
    						pthread_create(&car,NULL,carCreator,(void*)0);
							pthread_join(car,NULL);
								pthread_join(gui,NULL);
						break;
						//Custom car creation
						case SDLK_F2:
						//TODO
						break;
						//Ambulance creation
						case SDLK_F3:
    						pthread_create(&ambulance,NULL,carCreator,(void*)1);
							pthread_join(ambulance,NULL);
								pthread_join(gui,NULL);
						break;
						//Quits simulation
						case SDLK_ESCAPE:
						quit = 1;
						break;
					}
                    break;

                //SDL_QUIT event
                case SDL_QUIT:
                    quit = 1;
                    break;

                default:
                    break;
            }

        }

    }

    //Free the loaded image
    SDL_FreeSurface(threadville);

    //Quit SDL
    SDL_Quit();

    return 0;

}

//Exponential number calculator
float exponential(float media){
    float R;
    R = (float)rand()/(float)(RAND_MAX);
    return -media*log(R);
}
//Uniform number calculator
int uniform(int rangeL,int rangeH){
    int r = rand()%(rangeH-rangeL)+rangeL;
    return r;    
}

