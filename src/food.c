#include <stdlib.h>

#include "include/food.h"
#include "include/random.h"
#include "include/constants.h"

struct food *food_init(void)
{
  struct food *food = calloc(1, sizeof(struct food));
  return food;
}

void food_spawn(struct food *food)
{
  food->x = random_number_between(1, GAME_SIZE - 1);
  food->y = random_number_between(1, GAME_SIZE - 1);
}

void food_free(struct food *food)
{
  free(food);
}
