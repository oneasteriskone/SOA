#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <math.h>
#include "car.h"
// #include "configReader.h"
// #include "dijsktra.h"
// #include "readWeightMatrix.h"
// #include "usableSpaces.h"
#include "SDL/SDL.h"
// #include "readGUIMapping.h"

int quit = 0;
SDL_Surface* threadville = NULL;
SDL_Surface* screen = NULL;
SDL_Surface* carGUI = NULL;

SDL_Event event;


int carx = 0;
int cary = 0;

void carStart( car* c){
    int i = 0;
    while(1){
        i = 0;  
        for(i;i<25;i++)c->route[i] = -1;
        c->step = 0;
        int destiny = uniform(0,97);
        if(c->stops == 1) destiny = 97;
        //dijsktra(weightMatrix,c->position,destiny,c->route);
        //carMove(c);
        c->stops--;
        if(c->stops == 0){
            free(c);
            pthread_exit(NULL);            
        }
    }
}





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

	while(1){
		int i = 0;
		SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format,0,0,0));
		draw(screen, SDL_LoadBMP("blueCarLeft.bmp"), carx, cary);	

		carx+=1;
		cary+=1;

		printf("%s", "Moving cars!");

	}

	


	//while(1){
		// int i = 0;
		// for (i; i < 1230; i++) {

		// 	if(spaceList[i].usable == 1 && spaceList[i].busy == 1){
		// 		char* carName;
		// 		switch(spaceList[i].direction) {

		// 			case 0:
		// 			carName = strcpy(strcpy("images/", carNames[spaceList[i].model]), "Left.bmp");
		// 			printf("carName value:%s", carName);
		// 			carGUI = SDL_LoadBMP(carName);
		// 			break;

		// 			case 1:
		// 			carName = strcpy(strcpy("images/", carNames[spaceList[i].model]), "Right.bmp");
		// 			printf("carName value:%s", carName);
		// 			carGUI = SDL_LoadBMP(carName);
		// 			break;

		// 			case 2:
		// 			carName = strcpy(strcpy("images/", carNames[spaceList[i].model]), "Up.bmp");
		// 			printf("carName value:%s", carName);
		// 			carGUI = SDL_LoadBMP(carName);
		// 			break;

		// 			case 3:
		// 			carName = strcpy(strcpy("images/", carNames[spaceList[i].model]), "Down.bmp");
		// 			printf("carName value:%s", carName);
		// 			carGUI = SDL_LoadBMP(carName);
		// 			break;
		// 		}
				
		// 		draw(screen, carGUI, guiMapping[i][0],guiMapping[i][1]);
		// 		SDL_Delay(500);
		// 		SDL_Flip(screen);
		// 	}
		// }
	//}

}



int main(){
	pthread_t gui;

    //Start SDL
    SDL_Init(SDL_INIT_EVERYTHING);

    //Set up screen
    screen = SDL_SetVideoMode(831, 482, 24, SDL_SWSURFACE);

	//Enable Unicode translation
    SDL_EnableUNICODE(1);

    //Load image
    //threadville = SDL_LoadBMP("background_converted.bmp");

    //Apply image to screen
    SDL_BlitSurface(threadville, NULL, screen, NULL);
	SDL_Flip(screen);

	pthread_create(&gui,NULL,drawCars,NULL);

	
    while( !quit ){
        while( SDL_PollEvent(&event) ){
			pthread_t car;
			//pthread_t ambulance;
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
							printf("%s\n", "SDL!");	


							carx+=10;
							cary+=10;

							//drawCars();
						break;
						//Custom car creation
						case SDLK_F2:
						//TODO
						break;
						//Ambulance creation
						case SDLK_F3:
    						//pthread_create(&ambulance,NULL,carCreator,(void*)1);
							//pthread_join(ambulance,NULL);
							//	pthread_join(gui,NULL);
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