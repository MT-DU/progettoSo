#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <ncurses.h>
#include "../errors/errors.h"
#include <string.h>
/* */
#define NO_CURSOR 0
#define YES_CURSOR 1
#define STARSHIP {'\n',' ',' ','/','\\','\n','>','|',' ',' ','|','>','\n',' ',' ','\\','/','\n','\n'}
#define LOGO1 " ___ ___ ___ ___ ___    _| |___|  _|___ ___ _| |___ ___ ___ "
#define LOGO2 "|_ -| . | .'|  _| -_|  | . | -_|  _| -_|   | . | -_|  _|_ -|"
#define LOGO3 "|___|  _|__,|___|___|  |___|___|_| |___|_|_|___|___|_| |___|"
#define LOGO4 "    |_|                                                     "
#define SPACE_BETWEEN_CHOICES 3
#define PLAY_GAME_NUMBER 0
#define MODE_GAME_NUMBER 1
#define QUIT_GAME_NUMBER 2

#define PLAY_GAME "Play the game."
#define MODE_GAME "Select the Game Mode."
#define QUIT_GAME "Quit the game."

int divideByTwo(int coord);
void printMainMenu(WINDOW *win, int max_y, int max_x, int selection);
void hudMainMenu(WINDOW *win, int max_y, int max_x);
void mainMenu(WINDOW *win, int max_y, int max_x);
void keyPadSelector(WINDOW *win, int min_y, int max_y, int input, int* counter);
void printLogo(WINDOW *win, int max_x);
void setupColors();
void processStarShip();
void mainWindow();
void mainApp();

// gnome-terminal --geometry=widthxheight+Xposition+Yposition
// resize -s <rows> <columns>
