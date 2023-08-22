#include <stdio.h>
#include <stdlib.h>

#include "include/snake.h"
#include "include/constants.h"

struct snake *snake_init() {
  struct snake *snake = calloc(1, sizeof(struct snake));
  snake->x = 1;
  snake->y = 1;
  snake->xspeed = 1;
  snake->yspeed = 0;
  snake->tail_size = 0;

  // n-pairs of [x,y] will be stored
  snake->tail = (int **)malloc((MAX_TAIL_SIZE * 2) * sizeof(int *));

  return snake;
}

bool snake_eat(struct snake *snake, struct food *food) {
  if (snake->x == food->x && snake->y == food->y) {
    int *tail_piece = (int *)malloc(2 * sizeof(int));
    tail_piece[0] = snake->x;
    tail_piece[1] = snake->y;

    snake->tail[snake->tail_size++] = tail_piece;

    return true;
  }

  return false;
}

void snake_move(struct snake *snake) {
  if (snake->tail_size > 0)
  {
    for (int i = 0; i < snake->tail_size - 1; i++) {
      snake->tail[i][0] = snake->tail[i + 1][0];
      snake->tail[i][1] = snake->tail[i + 1][1];
    }

    snake->tail[snake->tail_size - 1][0] = snake->x;
    snake->tail[snake->tail_size - 1][1] = snake->y;
  }

  snake->x = snake->x + snake->xspeed;
  snake->y = snake->y + snake->yspeed;

  // If the edges are reached, come out of the other side
  if (snake->x == GAME_WIDTH - 1)
    snake->x = 1;
  else if (snake->x == 0)
    snake->x = GAME_WIDTH - 2;
  else if (snake->y == GAME_HEIGHT - 1)
    snake->y = 1;
  else if (snake->y == 0)
    snake->y = GAME_HEIGHT - 2;
}

bool snake_is_tail_piece(struct snake *snake, int x, int y) {
  if (snake->tail_size == 0)
    return false;

  for (int i = 0; i < snake->tail_size; i++) {
    if (snake->tail[i][0] == x && snake->tail[i][1] == y)
      return true;
  }

  return false;
}

void snake_direction(struct snake *snake, int xspeed, int yspeed) {
  snake->xspeed = xspeed;
  snake->yspeed = yspeed;
}

bool snake_overlaps_itself(struct snake *snake)
{
  for (int i = 0; i < snake->tail_size; i++)
  {
    if (snake->tail[i][0] == snake->x && snake->tail[i][1] == snake->y)
      return true;
  }

  return false;
}

void snake_free(struct snake *snake)
{
  for (int i = 0; i < snake->tail_size; i++)
    free(snake->tail[i]);

  free(snake->tail);
  free(snake);
}
