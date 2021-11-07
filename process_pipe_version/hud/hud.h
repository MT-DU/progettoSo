#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <ncurses.h>
#include "../errors/errors.h"

#define NO_CURSOR 0
#define YES_CURSOR 1
#define STARSHIP {'\n',' ',' ','/','\\','\n','>','|',' ',' ','|','>','\n',' ',' ','\\','/','\n','\n'}
#define LOGO {}
// potrebbe servire mettere un ' ' all'inizio in caso non si cancelli
void setupColors();
void processStarShip();
void createWindow();
void mainApp();


/*

 ___ ___ ___ ___ ___    _| |___|  _|___ ___ _| |___ ___ ___ 
|_ -| . | .'|  _| -_|  | . | -_|  _| -_|   | . | -_|  _|_ -|
|___|  _|__,|___|___|  |___|___|_| |___|_|_|___|___|_| |___|
    |_|

*/

