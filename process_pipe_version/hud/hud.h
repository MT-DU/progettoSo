#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <ncurses.h>
#include "../errors/errors.h"
#include <string.h>

#define NO_CURSOR 0
#define YES_CURSOR 1
#define STARSHIP {'\n',' ',' ','/','\\','\n','>','|',' ',' ','|','>','\n',' ',' ','\\','/','\n','\n'}
#define LOGO1 " ___ ___ ___ ___ ___    _| |___|  _|___ ___ _| |___ ___ ___ "
#define LOGO2 "|_ -| . | .'|  _| -_|  | . | -_|  _| -_|   | . | -_|  _|_ -|"
#define LOGO3 "|___|  _|__,|___|___|  |___|___|_| |___|_|_|___|___|_| |___|"
#define LOGO4 "    |_|                                                     "

void setupColors();
void processStarShip();
void mainWindow();
void mainApp();
void createLogo(char[300]);

// gnome-terminal --geometry=widthxheight+Xposition+Yposition
// resize -s <rows> <columns>
