#include "utility.h"

//#define STARSHIP {'\n',' ',' ','/','\\',' ',' ','\n','>','|','x',' ','|','>','\n',' ',' ','\\','/',' ',' ','\n'}
#define STARSHIP {{"  /\\ /"},{">|x |>"},{"  \\/ \\"}}
#define ENEMYSHIP {{"\\./"},{"<x>"},{"/^\\"}}

/* MACRO PER LE SHIP */
#define DEFAULT_X_ALLY_SHIP 2
#define ESCAPE_CODE '\033'
#define ASCII_CODE_A 65
#define ASCII_CODE_B 66
#define ASCII_CODE_SPACE_BAR 32
#define PASSO 1 
#define Y_HSEPARATOR 4
#define DIM_PIPE 2
#define NUMBER_ENEMY_SHIPS 6
#define ALLY_BORDER_SPACE 3
#define BLANK_SPACES_STARSHIP "      "
#define BLANK_SPACES_ALIEN "    "
#define BLANK_SPACE ' '

#define BULLET_SPRITE '+'
#define NUMBER_BULLETS 2
#define BOMB_SPRITE '@'
#define BULLET_PACE 4
#define MAX_ALIENS_FOR_ROW 3
#define MAX_BULLETS_ACTIVE 2
#define OBJ_ALIVE 1000
#define SPACE_BETWEEN_ALIENS 4

void mainGame(WINDOW* win, Point p);
void hudGame(WINDOW* win, Point p);
void mountainsBgEffect(WINDOW* win, Point p);
void allyShipController(WINDOW* win, Point p, int pipeOut);
void enemyShipController(WINDOW* win, Point p, int pipeOut, int idNumber);
void bulletController(WINDOW* win, Point p, Point posShip, Direction direction, int pipeOut);
void bombController(WINDOW* win, Point p, Point posAlien, int pipeOut);
void printObjects (WINDOW* win, Point p, int pipeIn);
Status checkCollision (WINDOW* win, Point p, Object* obj, Object array[], int size);
bool removeObject (Object array[], int size, pid_t pidObj);
bool isGameOver(/*Pensare a cosa metterci*/);
void printStarShip (WINDOW* win, Object ship);
void moveAllyShip (WINDOW* win, Point p, int* yPos, int* isBulletShot);
void printBullet (WINDOW* win, Object bullet);
bool checkPos (Point p, int yPos, int size);
void checkWindowCollision(WINDOW* win, Point p, Object* obj);