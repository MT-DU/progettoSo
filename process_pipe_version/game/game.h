#include "utility.h"

#define STARSHIP {{"  /\\ /"},{">|x |>"},{"  \\/ \\"}}
#define ENEMYSHIP {{"\\./"},{"<x>"},{"/^\\"}}
#define ENEMYSHIP_LEVEL_TWO {{"x-x"},{"| |"},{"x-x"}}

#define KILL_ALIEN 100
#define KILL_BOMB 101
#define KILL_ALLYSHIP 102
#define KILL_BULLET 103

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
#define TEXT_HUD_POS_Y 2
#define HEALTH_BAR_POS_X 2
#define HEALTH_TEXT_HUD "Vite rimanenti: "
#define HEALTH_SPACE_HUD "         "

void mainGame(WINDOW* win, Point p);
void hudGame(WINDOW* win, Point p, int allyShipHealth, int timer, int nAliens);
void mountainsBgEffect(WINDOW* win, Point p);
void allyShipController(WINDOW* win, Point p, int pipeOut);
void enemyShipController(WINDOW* win, Point p, int pipeOut, int idNumber);
void bulletController(WINDOW* win, Point p, Point posShip, Direction direction, int pipeOut);
void bombController(WINDOW* win, Point p, Point posAlien, int pipeOut);
void printObjects (WINDOW* win, Point p, int pipeIn);
Status checkCollision (WINDOW* win, Point p, Object* obj, Object array[], int size);
bool removeObject (Object array[], int size, pid_t pidObj);
bool isGameOver(int healthAllyShip, int alienAllyCollision);
void printStarShip (WINDOW* win, Point p, Object ship);
void moveAllyShip (WINDOW* win, Point p, int* yPos, int* isBulletShot);
void printBullet (WINDOW* win, Object bullet);
bool checkPos (Point p, int yPos, int size);
void checkWindowCollision(WINDOW* win, Point p, Object* obj);
void endGame(pid_t aliens[], pid_t allyShip);