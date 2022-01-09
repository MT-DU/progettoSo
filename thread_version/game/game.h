#include "utility.h"

// Macro per la stampa di tutti gli sprite
#define STARSHIP {{"  /\\ /"},{">|x |>"},{"  \\/ \\"}}
#define ENEMYSHIP {{"\\./"},{"<x>"},{"/^\\"}}
#define ENEMYSHIP_LEVEL_TWO {{"x-x"},{"| |"},{"x-x"}}
#define BULLET_SPRITE '+'
#define BOMB_SPRITE '@'

// Macro generiche
#define ASCII_CODE_SPACE_BAR 32
#define PASSO 1
#define NO_HEALTH_REMAINING 0

// Macro relative alla navicella alleta e ai proiettili
#define DEFAULT_X_ALLY_SHIP 2
#define NUMBER_BULLETS 2
#define NUMBER_ALLYSHIP 1
#define Y_HSEPARATOR 4 // Posizione verticale massima superiore della navicella alleta
#define BLANK_SPACES_STARSHIP "      "
#define BULLET_PACE 2

// Macro relative alla navicella aliena e alle bombe
#define BLANK_SPACES_ALIEN "    "
#define MAX_ALIENS_FOR_ROW 3
#define SPACE_BETWEEN_ALIENS 4
#define MIN_MOVE_ALIEN 1
#define MAX_MOVE_ALIEN 5
#define NUMBER_ENEMY_SHIPS_EASY 3
#define NUMBER_ENEMY_SHIPS_HARD 9

// Macro relative all'hud
#define TEXT_HUD_POS_Y 2
#define HEALTH_BAR_POS_X 2
#define NUMBER_ALIENS_BAR_POS_X 40
#define HEALTH_TEXT_HUD "Vite rimanenti: "
#define HEALTH_SPACE_HUD "         "
#define ALIENS_TEXT_HUD "Aliens remaining: %d"

// Dichiarazione delle funzioni/procedure utilizzate
void mainGame(WINDOW* win, Difficulty difficultyMode);
void hudGame(int allyShipHealth, int nAliens);
void* allyShipController();
void* bulletController();
void* enemyShipController (void* idNumberT);
void* bombController(void* posAlienV);
EndGame printObjects ();
void checkCollision();
void printStarShip (Object ship);
void printBullet (Object bullet);
void moveAllyShip ();
bool checkPos (Point p, int yPos, int size);
int getMaxAlien(Difficulty difficultyMode);
int getMaxHealth(Difficulty difficultyMode);
EndGame isGameOver (int nAliensAlive);
void initializeBuffers();
void killThread(pthread_t* thread, int* statusBuffer, int id);
void initializeObject(Object obj);
void freeAll();
