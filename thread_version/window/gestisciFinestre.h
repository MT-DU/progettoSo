#include <stdio.h>
#include <sys/wait.h>
#include <ncurses.h>
#include <string.h>

typedef struct point{
    int x;
    int y;
} Point;

#define DIM_MAX_PRINT_MENU 30
#define SELECTOR '>'
#define SPACE_BETWEEN_CHOICES 3
#define CREATE_OFFSET_SPACES 3

/* MACRO PER IL PAIR DEI COLORI */
#define PAIR_COLOR_LOGO 1
#define PAIR_COLOR_TEXT 2
#define PAIR_COLOR_ALLY_SHIP 3
#define PAIR_COLOR_ALIEN 4
#define PAIR_COLOR_BULLET 5
#define PAIR_COLOR_BOMB 6
#define PAIR_COLOR_HEART 7
#define PAIR_COLOR_ALIENS_REMAINING 8
#define PAIR_COLOR_ALIEN_HALF_HEALTH 9
#define PAIR_COLOR_ALIEN_LOW_HEALTH 10


#define MIN_POS 0

void keyPadSelector(WINDOW *win, int min_choice, int max_choice, int input, int* counter);
int searchLongestString(int row, char vetStrings[row][DIM_MAX_PRINT_MENU]);
int divideByTwo(int coord);
void printMenu (WINDOW* win, Point max_res, int selection, int menu, int row, char vetStringsMenu[row][DIM_MAX_PRINT_MENU]);
void setupColors();
void pickColor(WINDOW* win, int type_color);
void turnOffColor(WINDOW* win, int type_color);