#include "food.h"

#include <stdbool.h>
#include <stdlib.h>

#ifndef SNAKE_H
#define SNAKE_H

struct snake
{
  unsigned int x;
  unsigned int y;
  unsigned int xspeed;
  unsigned int yspeed;
  unsigned int tail_size;
  int **tail;
};

struct snake *snake_init();

bool snake_eat(struct snake *snake, struct food *food);

void snake_move(struct snake *snake);

bool snake_is_tail_piece(struct snake *snake, int x, int y);

void snake_direction(struct snake *snake, int xspeed, int yspeed);

bool snake_overlaps_itself(struct snake *snake);

void snake_free(struct snake *snake);

#endif /* !SNAKE_H */
