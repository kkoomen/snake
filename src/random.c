#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "include/random.h"

// Generate a number between range [from, to], exclusive.
int random_number_between(int from, int to)
{
  // Use time as the seed
  srand(time(NULL));

  return from + rand() % (to - from);
}
