#include <stdio.h>
#include <stdlib.h>

#include "include/constants.h"
#include "include/food.h"
#include "include/snake.h"

void _create_tail_piece(struct snake *snake, int x, int y)
{
  int *tail_piece = malloc(2 * sizeof(int));
  tail_piece[0] = x;
  tail_piece[1] = y;
  snake->tail[snake->tail_size++] = tail_piece;
}

struct snake *snake_init(void)
{
  struct snake *snake = calloc(1, sizeof(struct snake));
  snake->x = TAIL_START_SIZE + 1;
  snake->y = 1;
  snake->xspeed = 1;
  snake->yspeed = 0;
  snake->tail_size = 0;

  // n-pairs of [x,y] will be stored
  snake->tail = malloc((TAIL_MAX_SIZE + TAIL_START_SIZE) * 2 * sizeof(int *));

  for (unsigned int i = 1; i <= TAIL_START_SIZE; i++)
    _create_tail_piece(snake, i, 1);

  return snake;
}

bool snake_eat(struct snake *snake, struct food *food)
{
  if (snake->x == food->x && snake->y == food->y)
  {
    _create_tail_piece(snake, snake->x, snake->y);
    return true;
  }

  return false;
}

void snake_move(struct snake *snake)
{
  if (snake->tail_size > 0)
  {
    for (unsigned int i = 0; i < snake->tail_size - 1; i++)
    {
      snake->tail[i][0] = snake->tail[i + 1][0];
      snake->tail[i][1] = snake->tail[i + 1][1];
    }

    snake->tail[snake->tail_size - 1][0] = snake->x;
    snake->tail[snake->tail_size - 1][1] = snake->y;
  }

  snake->x = snake->x + snake->xspeed;
  snake->y = snake->y + snake->yspeed;

  // If the edges are reached, come out of the other side
  if (snake->x == GAME_SIZE - 1)
    snake->x = 1;
  else if (snake->x == 0)
    snake->x = GAME_SIZE - 2;
  else if (snake->y == GAME_SIZE - 1)
    snake->y = 1;
  else if (snake->y == 0)
    snake->y = GAME_SIZE - 2;
}

bool snake_is_tail_piece(const struct snake *snake, int x, int y)
{
  for (unsigned int i = 0; i < snake->tail_size; i++)
  {
    if (snake->tail[i][0] == x && snake->tail[i][1] == y)
      return true;
  }

  return false;
}

void snake_direction(struct snake *snake, int xspeed, int yspeed)
{
  snake->xspeed = xspeed;
  snake->yspeed = yspeed;
}

bool snake_overlaps_itself(const struct snake *snake)
{
  return snake_is_tail_piece(snake, snake->x, snake->y);
}

void snake_free(struct snake *snake)
{
  for (unsigned int i = 0; i < snake->tail_size; i++)
    free(snake->tail[i]);

  free(snake->tail);
  free(snake);
}
