#include "utility.h"

#define STARSHIP {{"  /\\ /"},{">|x |>"},{"  \\/ \\"}}
#define ENEMYSHIP {{"\\./"},{"<x>"},{"/^\\"}}
#define ENEMYSHIP_LEVEL_TWO {{"x-x"},{"| |"},{"x-x"}}

/* MACRO PER LA SHIP */
#define DEFAULT_X_ALLY_SHIP 2
#define ASCII_CODE_SPACE_BAR 32
#define BULLET_SPRITE '+'
#define NUMBER_BULLETS 2
#define PASSO 1 
#define Y_HSEPARATOR 4
#define DIM_PIPE 2
#define BLANK_SPACES_STARSHIP "      "
#define MAX_BULLETS_ACTIVE 2
#define BULLET_PACE 4

/* MACRO PER GLI ALIENI */
#define BOMB_SPRITE '@'
#define BLANK_SPACES_ALIEN "    "
#define MAX_ALIENS_FOR_ROW 3
#define OBJ_ALIVE 100
#define SPACE_BETWEEN_ALIENS 4
#define MIN_MOVE_ALIEN 1
#define MAX_MOVE_ALIEN 5

/* MACRO PER LA HUD */

#define TEXT_HUD_POS_Y 2
#define HEALTH_BAR_POS_X 2
#define TIMER_BAR_POS_X 40
#define NUMBER_ALIENS_BAR_POS_X 60
#define HEALTH_TEXT_HUD "Vite rimanenti: "
#define HEALTH_SPACE_HUD "         "
#define ALIENS_TEXT_HUD "Aliens remaining: %d"
#define ALIEN_TEXT_HUD "Alien remaining: %d"

/*DIFFICULTY: EASY*/
#define MAX_HEALTH_ALIEN 3
#define MAX_HEALTH_ALLY_EASY 3
#define NUMBER_ENEMY_SHIPS_EASY 3

/*DIFFICULTY: HARD*/
#define MAX_HEALTH_ALLY_HARD 1
#define NUMBER_ENEMY_SHIPS_HARD 9

void mainGame(WINDOW* win, Point p, Difficulty difficultyMode);
void hudGame(WINDOW* win, Point p, int allyShipHealth, int timer, int nAliens);
void mountainsBgEffect(WINDOW* win, Point p, int timer);
void allyShipController(WINDOW* win, Point p, int pipeOut);
void enemyShipController(WINDOW* win, Point p, int pipeOut, int idNumber, Difficulty difficultyMode);
void bulletController(WINDOW* win, Point p, Point posShip, Direction direction, int pipeOut);
void bombController(WINDOW* win, Point p, Point posAlien, int pipeOut);
void printObjects (WINDOW* win, Point p, int pipeIn, Difficulty difficultyMode);
bool removeObject (Object array[], int size, pid_t pidObj);
EndGame isGameOver(int healthAllyShip, bool alienAllyCollision, int nAliensAlive);
void printStarShip (WINDOW* win, Point p, Object ship);
void moveAllyShip (WINDOW* win, Point p, int* yPos, int* isBulletShot);
void printBullet (WINDOW* win, Object bullet);
bool checkPos (Point p, int yPos, int size);
void checkWindowCollision(WINDOW* win, Point p, Object* obj);
void endGame(pid_t aliens[], pid_t allyShip, Difficulty difficultyMode);
int getMaxAlien(Difficulty difficultyMode);
int getMaxHealth(Difficulty difficultyMode);