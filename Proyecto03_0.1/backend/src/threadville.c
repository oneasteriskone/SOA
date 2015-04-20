#include "SDL/SDL.h"
#include "streetsUtils.h"
#include "carUtils.h"
#include "pthread.h"

struct streetSpaceInfo* graph;
unsigned short graphLength;

struct vehicleInfo* cars[MAX_CARS];
int amountOfCars;

////////////////////testing

int quit = 0;
SDL_Surface* threadville = NULL;
SDL_Surface* screen = NULL;
SDL_Surface* carGUI = NULL;

SDL_Event event;

void *carCreator(void* isAmbulance);

/////////////////


void initialize()
{
    srand(time(0));
    graphLength = 0;
    amountOfCars = 0;
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

void addCar(){
  // Variables are initialized
  pthread_t car_t;
  struct vehicleInfo* car = createCar(graphLength, 0);
  // Car information is printed
  printVehicle(car);
  printf("AmountOfCars: %d\n", amountOfCars);
  // Car is added to the global array
  addCarToCarList(&car, &cars, amountOfCars);  
  // Thread
  pthread_create(&car_t,NULL,carCreator,(void*)0);
  pthread_join(car_t,NULL);
  // Update the car amount
  amountOfCars++;
}

int drawCars(){
  printf("%s\n", "Draw cars...");
  if(NULL == cars){
    printf("%s\n", "No cars have been added...");
    return 0;
  }
  int i = 0;
  while(i < amountOfCars){    
    printf("Car # %d\n", i);
    printVehicle(cars[i]);

    draw(screen, SDL_LoadBMP("blueCarLeft.bmp"), cars[i]->currentx, cars[i]->currenty);

    i++;
  }
  return 0;
}

void MoveCar(struct vehicleInfo car){
  //continue here -David
}

///

int main()
{
  initialize();

  //Start SDL
  SDL_Init(SDL_INIT_EVERYTHING);

  //Set up screen
  screen = SDL_SetVideoMode(831, 482, 24, SDL_SWSURFACE);

  //Enable Unicode translation
  SDL_EnableUNICODE(1);
  
    while( !quit ){
      while( SDL_PollEvent(&event) ){
        
        SDL_BlitSurface(threadville, NULL, screen, NULL);
        SDL_Flip(screen);

        switch(event.type){
            case SDL_KEYUP:
                switch(event.key.keysym.sym){
                  //random car creation
                  case SDLK_F1:
                    addCar();              
                    //////
                    pthread_t gui; 
                    pthread_create(&gui,NULL,drawCars,NULL);
                    pthread_join(gui,NULL);
                    //////
                    printf("%s\n", "SDL!"); //test
                  break;

                  case SDLK_F2:
                    printf("%d\n", amountOfCars); //test
                  break;

                  case SDLK_F3:
                    printf("%d\n", cars[0]->color); //test
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

    // ENDS GUI TESTING
    finish();
    return 0;

}