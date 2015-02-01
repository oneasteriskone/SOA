#ifndef MISC_UTILS
#define MISC_UTILS

#include <math.h>
#include <stdlib.h>
#include <time.h>

float genRand()
{
  return( (float) rand()/RAND_MAX );
}


int exponentialRand(float lambda)
{
  float u,x;
  u=genRand();
  x=(-1/lambda)*log(u);
  return x;

}

#endif
