#ifndef FOOD_H
#define FOOD_H

#include <stdlib.h>

struct food
{
  unsigned int x;
  unsigned int y;
};

struct food *food_init();

void food_spawn(struct food *food);

void food_free(struct food *food);

#endif /* !FOOD_H */
