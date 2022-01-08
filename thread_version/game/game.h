#include "utility.h"

#define STARSHIP {{"  /\\ /"},{">|x |>"},{"  \\/ \\"}}
#define ENEMYSHIP {{"\\./"},{"<x>"},{"/^\\"}}
#define ENEMYSHIP_LEVEL_TWO {{"x-x"},{"| |"},{"x-x"}}

/* MACRO PER LA SHIP */
#define DEFAULT_X_ALLY_SHIP 2
#define ASCII_CODE_SPACE_BAR 32
#define BULLET_SPRITE '+'
#define NUMBER_BULLETS 2
#define NUMBER_ALLYSHIP 1
#define PASSO 1 
#define Y_HSEPARATOR 4
#define DIM_PIPE 2
#define BLANK_SPACES_STARSHIP "      "
#define MAX_BULLETS_ACTIVE 2
#define BULLET_PACE 2
#define NO_HEALTH_REMAINING 0

/* MACRO PER GLI ALIENI */
#define BOMB_SPRITE '@'
#define BLANK_SPACES_ALIEN "    "
#define MAX_ALIENS_FOR_ROW 3
#define SPACE_BETWEEN_ALIENS 4
#define MIN_MOVE_ALIEN 1
#define MAX_MOVE_ALIEN 5

/* MACRO PER LA HUD */

#define TEXT_HUD_POS_Y 2
#define HEALTH_BAR_POS_X 2
#define NUMBER_ALIENS_BAR_POS_X 40
#define HEALTH_TEXT_HUD "Vite rimanenti: "
#define HEALTH_SPACE_HUD "         "
#define ALIENS_TEXT_HUD "Aliens remaining: %d"
#define ALIEN_TEXT_HUD "Alien remaining: %d"

/*DIFFICULTY: EASY*/
#define MAX_HEALTH_ALIEN 3
#define MAX_HEALTH_ALLY_EASY 3
#define NUMBER_ENEMY_SHIPS_EASY 3
#define DELAY_ALIEN_EASY 200000

/*DIFFICULTY: HARD*/
#define MAX_HEALTH_ALLY_HARD 1
#define NUMBER_ENEMY_SHIPS_HARD 9
#define DELAY_ALIEN_HARD 400000


void mainGame(WINDOW* win, Difficulty difficultyMode);
void hudGame(int allyShipHealth, int nAliens);
void* allyShipController();
void* enemyShipController (void* idNumberT);
void* bulletController();
void* bombController(void* posAlienV);
EndGame printObjects ();
EndGame isGameOver (int nAliensAlive);
void printStarShip (Object ship);
void checkCollision();
void moveAllyShip ();
void printBullet (Object bullet);
bool checkPos (Point p, int yPos, int size);
void checkWindowCollision(WINDOW* win, Point p, Object* obj);
void endGame(pthread_t aliens[], pthread_t allyShip);
int getMaxAlien(Difficulty difficultyMode);
int getMaxHealth(Difficulty difficultyMode);
int getDelay(Difficulty difficultyMode);
void endGamePrint(WINDOW* win, Point p, EndGame gameStatus);
int getMaxAlien(Difficulty difficultyMode);
void initializeBuffers();
void killThread(pthread_t* thread, int* statusBuffer, int id);
void initializeObject(Object obj);
void freeAll();