#include "../window/gestisciFinestre.h"
#include "../errors/errors.h"
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <fcntl.h>

//#define STARSHIP {'\n',' ',' ','/','\\',' ',' ','\n','>','|','x',' ','|','>','\n',' ',' ','\\','/',' ',' ','\n'}
#define STARSHIP {{"  /\\  "},{">|x |>"},{"  \\/  "}}
#define ENEMYSHIP {{"\\./"},{"<x>"},{"/^\\"}}

#define ROWS_STARSHIP 3
#define COLS_STARSHIP 6
#define OUTER_STARSHIP 2
#define STARSHIP_SIZE 3

#define ROWS_ALIEN 3
#define COLS_ALIEN 3
#define OUTER_ALIEN 1
#define ALIEN_SIZE 3

#define MOUNTAINS_ROWS 8
#define MOUNTAINS_COLS 21

#define MIN_RAND 0
#define MAX_RAND 100

typedef enum{
    ALLY_SHIP_TYPE,
    ENEMY_SHIP_TYPE,
    BIG_ENEMY_SHIP_TYPE,
    BULLET_TYPE,
    BOMB_TYPE
} TypeObject;

typedef enum{
    UP_DIRECTION, DOWN_DIRECTION
} Direction;

typedef struct{
    Point pos;
    pid_t pid;
    TypeObject typeObject;
    Direction direction;
} Object;

/* MACRO PER LE SHIP */
#define DEFAULT_X_ALLY_SHIP 2
#define ESCAPE_CODE '\033'
#define ASCII_CODE_A 65
#define ASCII_CODE_B 66
#define ASCII_CODE_SPACE_BAR 32
#define PASSO 1
#define Y_HSEPARATOR 4
#define DIM_PIPE 2
#define NUMBER_ENEMY_SHIPS 2
#define ALLY_BORDER_SPACE 3
#define BLANK_SPACES_STARSHIP "      "
#define BLANK_SPACES_ALIEN "    "
#define BLANK_SPACE ' '

#define BULLET_SPRITE '+'
#define NUMBER_BULLETS 2
#define BOMB_SPRITE '@'
#define BULLET_PACE 4
#define MAX_BULLETS_ACTIVE 5
#define MAX_ALIENS_FOR_ROW 5

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
void enemyShipController(WINDOW* win, Point p, int pipeOut, int idNumber);
void bulletController(WINDOW* win, Point p, Point posShip, Direction direction, int pipeOut, int* nBulletsActive);
void bombController(WINDOW* win, Point p, Point posAlien, int pipeOut);
void printObjects(WINDOW* win, Point p, int pipeIn);
void checkCollision (WINDOW* win, Point p, int pipeIn, int pipeOutPrint, int pipeInAliens[NUMBER_ENEMY_SHIPS][DIM_PIPE]);
bool isGameOver(/*Pensare a cosa metterci*/);
void printStarShip (WINDOW* win, Object ship);
void moveAllyShip (WINDOW* win, Point p, int* yPos, int* isBulletShot);
void printBullet (WINDOW* win, Object bullet);
bool checkPos (Point p, int yPos, int size);