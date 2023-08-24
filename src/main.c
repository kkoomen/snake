#include <unistd.h>
#include <stdio.h>
#include <termios.h>
#include <pthread.h>

#include "include/snake.h"
#include "include/food.h"
#include "include/constants.h"

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int input_char = -1;

void disable_buffering()
{
  struct termios term;
  tcgetattr(STDIN_FILENO, &term);
  term.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &term);
}

void enable_buffering()
{
  struct termios term;
  tcgetattr(STDIN_FILENO, &term);
  term.c_lflag |= ICANON | ECHO;
  tcsetattr(STDIN_FILENO, TCSANOW, &term);
}

void term_clear()
{
  printf("\033[2J");
  printf("\033[%d;%dH", 0, 0);
}

void *get_input_char_thread(void *arg)
{
  disable_buffering();

  int ch;
  while ((ch = getchar()))
  {
    pthread_mutex_lock(&mutex);
    input_char = ch;
    pthread_mutex_unlock(&mutex);
  }

  enable_buffering();
  return NULL;
}

void draw_filled_square(char *text)
{
  printf("\x1B[48;5;15m"); // set background to white
  printf("\x1B[38;5;0m");  // set foreground to black
  printf("%s", text);
  printf("\x1B[0m");       // reset colors
}

void draw_board(struct snake *snake, struct food *food)
{
  for (unsigned int y = 0; y < GAME_SIZE; y++)
  {
    for (unsigned int x = 0; x < GAME_SIZE; x++)
    {
      if (x == 0 && y == 0)
        printf(" \u250C"); // top left corner
      else if (x == GAME_SIZE - 1 && y == 0)
        printf("\u2510 "); // top right corner
      else if (x == GAME_SIZE - 1 && y == GAME_SIZE - 1)
        printf("\u2518 "); // bottom right corner
      else if (x == 0 && y == GAME_SIZE - 1)
        printf(" \u2514"); // bottom left corner
      else if (y == 0 || y == GAME_SIZE - 1)
        printf("\u2500\u2500\u2500"); // horizontal borders
      else if (x == 0)
        printf(" \u2502"); // vertical borders left side
      else if (x == GAME_SIZE - 1)
        printf("\u2502 "); // vertical borders right side
      else if ((x == snake->x && y == snake->y)) {
        // snake head marker
        if (snake->xspeed == SNAKE_XSPEED)
          draw_filled_square(" \u2022 "); // moving right
        else if (snake->xspeed == -SNAKE_XSPEED)
          draw_filled_square(" \u2022 "); // moving left
        else if (snake->yspeed == SNAKE_YSPEED)
          draw_filled_square("\u2022 \u2022"); // moving down
        else if (snake->yspeed == -SNAKE_YSPEED)
          draw_filled_square("\u2022 \u2022"); // moving up
      }
      else if (snake_is_tail_piece(snake, x, y))
        draw_filled_square("   "); // snake tail marker
      else if (x == food->x && y == food->y)
        printf(" \u2022 "); // food marker
      else
        printf("   "); // print spaces for alignment

      if (x == GAME_SIZE - 1) {
        if (y == 1)
          printf("  ███████ ███    ██  █████  ██   ██ ███████ ");
        if (y == 2)
          printf("  ██      ████   ██ ██   ██ ██  ██  ██      ");
        if (y == 3)
          printf("  ███████ ██ ██  ██ ███████ █████   █████   ");
        if (y == 4)
          printf("       ██ ██  ██ ██ ██   ██ ██  ██  ██      ");
        if (y == 5)
          printf("  ███████ ██   ████ ██   ██ ██   ██ ███████ ");
        if (y == 7)
          printf("  Made by Kim 金可明");
        if (y == 8)
          printf("  https://github.com/kkoomen/snake");

        if (y == GAME_SIZE - 2)
          printf("  Score: %d/%d", snake->tail_size, MAX_TAIL_SIZE);
      }

    }
    // print newline for each row
    printf("\n");
  }
}

void handle_input(int ch, struct snake *snake)
{
  if (ch != -1)
  {
    switch (ch)
    {
      case 'A':
        // UP ARROW
        // we can only go up if we're not going down
        if (snake->yspeed != SNAKE_YSPEED)
          snake_direction(snake, 0, -SNAKE_YSPEED);
        break;
      case 'B':
        // DOWN ARROW
        // we can only go down if we're not going up
        if (snake->yspeed != -SNAKE_YSPEED)
          snake_direction(snake, 0, SNAKE_YSPEED);
        break;
      case 'C':
        // RIGHT ARROW
        // we can only go right if we're not going left
        if (snake->xspeed != -SNAKE_XSPEED)
          snake_direction(snake, SNAKE_XSPEED, 0);
        break;
      case 'D':
        // LEFT ARROW
        // we can only go left if we're not going right
        if (snake->xspeed != SNAKE_XSPEED)
          snake_direction(snake, -SNAKE_XSPEED, 0);
        break;
    }
  }
}

int main(void)
{
  struct snake *snake = snake_init();
  struct food *food = food_init();

  pthread_t thread;
  pthread_create(&thread, NULL, get_input_char_thread, NULL);

  int frameCount = 0;
  while (1)
  {
    term_clear();

    draw_board(snake, food);

    if (snake->tail_size == MAX_TAIL_SIZE)
    {
      printf("You have completed snake, congratulations!\n");
      break;
    }

    pthread_mutex_lock(&mutex);
    int ch = input_char;
    input_char = -1;
    pthread_mutex_unlock(&mutex);
    handle_input(ch, snake);

    if (snake_eat(snake, food))
      food_spawn(food);

    snake_move(snake);

    if (snake_overlaps_itself(snake))
    {
      printf("Oh, oh... you died with a score of %d/%d.\n", snake->tail_size, MAX_TAIL_SIZE);
      break;
    }

    usleep(1000000 / GAME_FRAMERATE);
  }

  snake_free(snake);
  food_free(food);
  return 0;
}
