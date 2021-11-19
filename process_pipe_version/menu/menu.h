#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <ncurses.h>
#include "../errors/errors.h"
#include <string.h>

typedef struct point{
    int x;
    int y;
}Point;

/* DEFINIZIONE MACRO */
#define NO_CURSOR 0
#define YES_CURSOR 1
#define LOGO1 " ___ ___ ___ ___ ___    _| |___|  _|___ ___ _| |___ ___ ___ "
#define LOGO2 "|_ -| . | .'|  _| -_|  | . | -_|  _| -_|   | . | -_|  _|_ -|"
#define LOGO3 "|___|  _|__,|___|___|  |___|___|_| |___|_|_|___|___|_| |___|"
#define LOGO4 "    |_|                                                     "
#define SIZE_LOGO 61 // lunghezza della stringa della singola macro logo
#define NUM_LOGHI 4 // tiene conto del numero di macro per i logo ci sono
#define SPACE_BETWEEN_CHOICES 3
#define PLAY_GAME_NUMBER 0
#define MODE_GAME_NUMBER 1
#define QUIT_GAME_NUMBER 2
#define ASCII_CODE_Q 113
#define ASCII_CODE_ENTER 10
#define SELECTOR ">"
#define PAIR_COLOR_LOGO 1
#define PAIR_COLOR_TEXT 2
#define Y_HLINE 6
#define PLAY_GAME "Play the game."
#define MODE_GAME "Select the Game Mode."
#define QUIT_GAME "Quit the game."
#define MIN_POS 0 

/* PROTOTIPI FUNZIONI / PROCEDURE */
void mainApp();
void mainWindow();
void mainMenu(WINDOW *win, Point max_res);
void hudMainMenu(WINDOW *win, Point max_res);
void printMainMenu(WINDOW *win, Point max_res, int selection);
void keyPadSelector(WINDOW *win, int min_choice, int max_choice, int input, int* counter);
void selectOptionMainMenu(WINDOW* win, Point max_res, int* input, int counter);
void printLogo(WINDOW *win, Point max_res);
void pickColor(WINDOW* win, int type_color);
int divideByTwo(int coord);
void setupColors();
void processStarShip();



// gnome-terminal --geometry=widthxheight+Xposition+Yposition
// resize -s <rows> <columns>
// amogus: ඩ
