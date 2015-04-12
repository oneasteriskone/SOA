#include <math.h>

int uniform(int rangeL,int rangeH){
    int r = rand()%(rangeH-rangeL)+rangeL;
    return r;    
}