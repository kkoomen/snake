#include <ncurses.h>
#include <locale.h>
#include <unistd.h>
#include <stdio.h>

#include "include/constants.h"
#include "include/food.h"
#include "include/snake.h"

volatile sig_atomic_t ctrl_c_pressed = 0;

void draw_board(const struct snake *snake, const struct food *food)
{
  clear();

  for (unsigned int y = 0; y < GAME_SIZE; y++)
  {
    for (unsigned int x = 0; x < GAME_SIZE; x++)
    {
      if (x == 0 && y == 0)
        printw(" \u250C"); // top left corner
      else if (x == GAME_SIZE - 1 && y == 0)
        printw("\u2510 "); // top right corner
      else if (x == GAME_SIZE - 1 && y == GAME_SIZE - 1)
        printw("\u2518 "); // bottom right corner
      else if (x == 0 && y == GAME_SIZE - 1)
        printw(" \u2514"); // bottom left corner
      else if (y == 0 || y == GAME_SIZE - 1)
        printw("\u2500\u2500\u2500"); // horizontal borders
      else if (x == 0)
        printw(" \u2502"); // vertical borders left side
      else if (x == GAME_SIZE - 1)
        printw("\u2502 "); // vertical borders right side
      else if ((x == snake->x && y == snake->y))
      {
        // snake head marker
        if (snake->xspeed != 0)
        {
          // moving left or right
          attron(COLOR_PAIR(1));
          printw(" ");
          printw("\u2022");
          printw(" ");
          attroff(COLOR_PAIR(1));
        }
        else if (snake->yspeed != 0)
        {
          // moving up or down
          attron(COLOR_PAIR(1));
          printw("\u2022");
          printw(" ");
          printw("\u2022");
          attroff(COLOR_PAIR(1));
        }
      }
      else if (snake_is_tail_piece(snake, x, y))
      {
        attron(COLOR_PAIR(1));
        printw("   ");
        attroff(COLOR_PAIR(1));
      }
      else if (x == food->x && y == food->y)
        printw(" \u2022 "); // food marker
      else
        printw("   "); // print spaces for alignment

      // print additional info
      if (x == GAME_SIZE - 1)
      {
        if (y == 1) printw("  ███████ ███    ██  █████  ██   ██ ███████ ");
        if (y == 2) printw("  ██      ████   ██ ██   ██ ██  ██  ██      ");
        if (y == 3) printw("  ███████ ██ ██  ██ ███████ █████   █████   ");
        if (y == 4) printw("       ██ ██  ██ ██ ██   ██ ██  ██  ██      ");
        if (y == 5) printw("  ███████ ██   ████ ██   ██ ██   ██ ███████ ");
        if (y == 7) printw("  Made by Kim 金可明");
        if (y == 8) printw("  https://github.com/kkoomen/snake");

        if (y == GAME_SIZE - 2) printw("  Score: %d/%d", snake->tail_size - TAIL_START_SIZE, TAIL_MAX_SIZE);
      }
    }
    printw("\n");
  }
}

void handle_input(int ch, struct snake *snake)
{
  switch (ch)
  {
    case KEY_UP:
      // we can only go up if we're not going down
      if (snake->yspeed != SNAKE_YSPEED)
        snake_direction(snake, 0, -SNAKE_YSPEED);
      break;
    case KEY_DOWN:
      // we can only go down if we're not going up
      if (snake->yspeed != -SNAKE_YSPEED)
        snake_direction(snake, 0, SNAKE_YSPEED);
      break;
    case KEY_RIGHT:
      // we can only go right if we're not going left
      if (snake->xspeed != -SNAKE_XSPEED)
        snake_direction(snake, SNAKE_XSPEED, 0);
      break;
    case KEY_LEFT:
      // we can only go left if we're not going right
      if (snake->xspeed != SNAKE_XSPEED)
        snake_direction(snake, -SNAKE_XSPEED, 0);
      break;
  }
}

void sigint_handler(int signum __attribute__((unused)))
{
  ctrl_c_pressed = 1;
}

void setup_ncurses(void)
{
  setlocale(LC_ALL, ""); // Set the locale to allow Unicode characters
  initscr();
  cbreak();              // disable line buffering
  noecho();              // don't echo user input
  nodelay(stdscr, TRUE); // make getch() non-blocking
  keypad(stdscr, TRUE);  // enable function keys

  // register sigint handler
  signal(SIGINT, sigint_handler);

  // enable term colors
  start_color();
  use_default_colors();
  init_pair(1, COLOR_BLACK, COLOR_WHITE);
}

void create_food(struct food *food, const struct snake *snake)
{
  food_spawn(food);

  while (
    (food->x == snake->x && food->y == snake->y) ||
    snake_is_tail_piece(snake, food->x, food->y)
  ) food_spawn(food);
}

int main(void)
{
  struct snake *snake = snake_init();
  struct food *food = food_init();
  useconds_t frame_delay = 1000000 / GAME_FRAMERATE;

  setup_ncurses();

  create_food(food, snake);

  while (1)
  {
    int ch = getch();
    if (ch != ERR)
      handle_input(ch, snake);

    if (ctrl_c_pressed) break;

    draw_board(snake, food);

    if (snake_eat(snake, food))
    {
      if (snake->tail_size - TAIL_START_SIZE == TAIL_MAX_SIZE)
      {
        draw_board(snake, food);
        printw("You have completed snake, congratulations!\n");
        break;
      }

      create_food(food, snake);
    }

    snake_move(snake);

    if (snake_overlaps_itself(snake))
    {
      printw("Oh, oh... you died!\n");
      break;
    }

    usleep(frame_delay);
    refresh();
  }

  if (!ctrl_c_pressed)
  {
    printw("Press 'q' to quit\n");
    nodelay(stdscr, FALSE);
    while (1)
    {
      int ch = getch();
      if (ch == 'q' || ch == ERR)
        break;
    }
    endwin();
  }
  else endwin();

  snake_free(snake);
  food_free(food);
  return 0;
}
