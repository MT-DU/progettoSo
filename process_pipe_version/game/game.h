#include "../window/gestisciFinestre.h"
#include "../errors/errors.h"
#include <unistd.h>

//#define STARSHIP {'\n',' ',' ','/','\\',' ',' ','\n','>','|','x',' ','|','>','\n',' ',' ','\\','/',' ',' ','\n'}
#define STARSHIP {{"  /\\  "},{">|x |>"},{"  \\/  "}}

#define ROWS_STARSHIP 3
#define COLS_STARSHIP 6
#define OUTER_STARSHIP 2

typedef struct ship{
    Point pos;
    char sprite[ROWS_STARSHIP][COLS_STARSHIP];
} Ship;

/* MACRO PER ALLY SHIP */
#define DEFAULT_X_ALLY_SHIP 2
#define ESCAPE_CODE '\033'
#define ASCII_CODE_A 65
#define ASCII_CODE_B 66
#define PASSO 1
#define Y_HSEPARATOR 4
#define DIM_PIPE 2
#define NUMBER_ENEMY_SHIPS 4
#define STARSHIP_SIZE 3
#define ALLY_BORDER_SPACE 3
#define BLANK_SPACE "      "
#define BULLET '+'

/* MACRO PER GESTIRE I PROCESSI */
#define PROCESS_RETURN_FAILURE -1
#define PROCESS_RETURN_CHILD_PID 0

/* MACRO PER GESTIRE LE PIPE */
#define PIPE_READ 0
#define PIPE_WRITE 1

void mainGame(WINDOW* win, Point p);
void hudGame(WINDOW* win, Point p);
void mountainsBgEffect(WINDOW* win, Point p);
void allyShipController(WINDOW* win, Point p, int pipeOut);
void enemyShipController(WINDOW* win, Point p, int pipeOut);
void bulletController(WINDOW* win, Point p, int pipeOut);
void bombController(WINDOW* win, Point p, int pipeOut);
void printObjects(WINDOW* win, Point p, int pipeIn);
void checkCollision(WINDOW* win, Point p, int pipeIn);
bool isGameOver(/*Pensare a cosa metterci*/);

void printStarShip (WINDOW* win, Ship ship);
void moveAllyShip (WINDOW* win, Point p, int* yPos);
bool checkPos (Point p, int yPos);