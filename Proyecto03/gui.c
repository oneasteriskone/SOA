#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "SDL/SDL.h"
#include "readGUIMapping.h"

int draw(SDL_Surface* Surf_Dest, SDL_Surface* Surf_Src, int X, int Y) {
	if(Surf_Dest == NULL || Surf_Src == NULL) {
		return -1;
	}

	SDL_Rect DestR;

	DestR.x = X;
	DestR.y = Y;

	SDL_BlitSurface(Surf_Src, NULL, Surf_Dest, &DestR);

	return 0;
}

int main() {

	SDL_Surface* threadville = NULL;
	SDL_Surface* screen = NULL;
	SDL_Surface* carGUI = NULL;
	SDL_Event event;
	int quit = 0;
	int guiMapping [1230][2];

	readGUIMapping(guiMapping);
    //Start SDL
    SDL_Init(SDL_INIT_EVERYTHING);

    //Set up screen
    screen = SDL_SetVideoMode(888, 925, 24, SDL_SWSURFACE);

	//Enable Unicode translation
    SDL_EnableUNICODE(1);

    //Load image
    threadville = SDL_LoadBMP("images/threadville.bmp");

    //Apply image to screen
    SDL_BlitSurface(threadville, NULL, screen, NULL);
	SDL_Flip(screen);
			
				

int i = 41;
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
				carGUI = SDL_LoadBMP("images/blueCarLeft.bmp");
				draw(screen, carGUI, guiMapping[i][0],guiMapping[i][1]);
				i++;
				SDL_Flip(screen);
						break;
						//Custom car creation
						case SDLK_F2:
						//TODO
						break;
						//Ambulance creation
						case SDLK_F3:
				carGUI = SDL_LoadBMP("images/ambulanceLeft.bmp");
				draw(screen, carGUI, guiMapping[i][0],guiMapping[i][1]);
				i++;
				SDL_Flip(screen);
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

}
