#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <ncurses.h>
#include "../errors/errors.h"
#include <string.h>

typedef struct point{
    int x;
    int y;
} Point;

/* DEFINIZIONE MACRO */
#define NO_CURSOR 0
#define YES_CURSOR 1
#define ASCII_CODE_Q 113
#define ASCII_CODE_ENTER 10
#define SELECTOR ">"
#define PAIR_COLOR_LOGO 1
#define PAIR_COLOR_TEXT 2
#define NUMBER_CHOICES 3 //indica il numero di scelte per menu, da modificare nel caso in cui si aumentino i casi dei vari menu

//macro gestione hud
#define Y_HLINE 6
#define MIN_POS 0
#define LOGO1 " ___ ___ ___ ___ ___    _| |___|  _|___ ___ _| |___ ___ ___ "
#define LOGO2 "|_ -| . | .'|  _| -_|  | . | -_|  _| -_|   | . | -_|  _|_ -|"
#define LOGO3 "|___|  _|__,|___|___|  |___|___|_| |___|_|_|___|___|_| |___|"
#define LOGO4 "    |_|                                                     "
#define SIZE_LOGO 61 // lunghezza della stringa della singola macro logo
#define NUM_LOGHI 4 // tiene conto del numero di macro per i logo ci sono
#define SPACE_BETWEEN_CHOICES 3
#define CREATE_OFFSET_SPACES 3
#define NUMBER_MENUS 2 //numero di menu implementati nel software
//macro gestione menu principale
#define PLAY_GAME_NUMBER 0
#define MODE_GAME_NUMBER 1
#define QUIT_GAME_NUMBER 2
#define PLAY_GAME "Play the game."
#define MODE_GAME "Select the Game Mode."
#define QUIT_GAME "Quit the game."
#define PRINT_MENU_MAIN 0 

//macro gestione menu difficolta
#define EASY_MODE_NUMBER 0
#define HARD_MODE_NUMBER 1
#define CUSTOM_MODE_NUMBER 2
#define EASY_MODE "Modalita' Facile"
#define HARD_MODE "Modalita' Difficile"
#define CUSTOM_MODE "Personalizza"
#define PRINT_MENU_GAME_MODE 1 
#define DIM_MAX_PRINT_MENU 30

/* PROTOTIPI FUNZIONI / PROCEDURE */
void mainApp();
void setupColors();
void mainWindow();
void mainMenu(WINDOW *win, Point max_res);
void hudMainMenu(WINDOW *win, Point max_res);
void pickColor(WINDOW* win, int type_color);
void printLogo(WINDOW *win, Point max_res);
void keyPadSelector(WINDOW *win, int min_choice, int max_choice, int input, int* counter);
void selectOptionMainMenu(WINDOW* win, Point max_res, int* input, int counter);
int divideByTwo(int coord);
void gameMode (WINDOW* win, Point max_res);
void printMenu (WINDOW* win, Point max_res, int selection, int menu);
int searchLongestString(int row, char vetStrings[row][DIM_MAX_PRINT_MENU], int start, int end);

