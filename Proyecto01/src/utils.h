#ifndef MISC_UTILS
#define MISC_UTILS

#include <math.h>

#define MEDIA 0.05

int exponentialRand(double lambda)
{
  double x = (double)rand();
  double value = 1.0 - exp(-1 * lambda * x);
  return (int)value;
}

#endif
