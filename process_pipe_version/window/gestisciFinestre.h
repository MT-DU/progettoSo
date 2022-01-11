#include <stdio.h>
#include <sys/wait.h>
#include <ncurses.h>
#include <string.h>

// Dichiarazione della struttura usata per passare la risoluzione e le posizioni degli oggetti
typedef struct point{
    int x;
    int y;
} Point;

// Macro relative alle stampe del menu'
#define DIM_MAX_PRINT_MENU 30
#define SELECTOR '>'
#define SPACE_BETWEEN_CHOICES 3
#define CREATE_OFFSET_SPACES 3

// Macro relative all'inizializzazione dei colori
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
#define PAIR_COLOR_MAIN_MENU 11
#define MIN_POS 0

// Dichiarazione delle funzioni e procedure utilizzate 
void keyPadSelector(int min_choice, int max_choice, int input, int* counter);
void printMenu (WINDOW* win, Point max_res, int selection, int row, char vetStringsMenu[row][DIM_MAX_PRINT_MENU]);
int searchLongestString(int row, char vetStrings[row][DIM_MAX_PRINT_MENU]);
int divideByTwo(int coord);
void setupColors();
void pickColor(WINDOW* win, int type_color);
void turnOffColor(WINDOW* win, int type_color);