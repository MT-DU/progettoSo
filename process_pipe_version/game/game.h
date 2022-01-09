#include "utility.h"

// Macro per la stampa di tutti gli sprite
#define STARSHIP {{"  /\\ /"},{">|x |>"},{"  \\/ \\"}}
#define ENEMYSHIP {{"\\./"},{"<x>"},{"/^\\"}}
#define ENEMYSHIP_LEVEL_TWO {{"x-x"},{"| |"},{"x-x"}}
#define BULLET_SPRITE '+'
#define BOMB_SPRITE '@'

// Macro generiche
#define ASCII_CODE_SPACE_BAR 32
#define DIM_PIPE 2
#define PASSO 1
#define NO_HEALTH_REMAINING 0
#define OBJ_ALIVE 100

// Macro relative alla navicella alleta e ai proiettili
#define DEFAULT_X_ALLY_SHIP 2
#define NUMBER_BULLETS 2
#define Y_HSEPARATOR 4 // Posizione verticale massima superiore della navicella alleta
#define BLANK_SPACES_STARSHIP "      "
#define BULLET_PACE 2

// Macro relative alla navicella aliena e alle bombe
#define BLANK_SPACES_ALIEN "    "
#define MAX_ALIENS_FOR_ROW 3
#define SPACE_BETWEEN_ALIENS 4
#define MIN_MOVE_ALIEN 1
#define MAX_MOVE_ALIEN 5
#define MAX_HEALTH_ALIEN 3

// Macro relative all'hud
#define TEXT_HUD_POS_Y 2
#define HEALTH_BAR_POS_X 2
#define NUMBER_ALIENS_BAR_POS_X 40
#define HEALTH_TEXT_HUD "Vite rimanenti: "
#define HEALTH_SPACE_HUD "         "
#define ALIENS_TEXT_HUD "Aliens remaining: %d"

// Macro per la modalita' facile
#define MAX_HEALTH_ALLY_EASY 3
#define NUMBER_ENEMY_SHIPS_EASY 3
#define DELAY_ALIEN_EASY 200000

// Macro per la modalita' difficile
#define MAX_HEALTH_ALLY_HARD 1
#define NUMBER_ENEMY_SHIPS_HARD 9
#define DELAY_ALIEN_HARD 400000

// Macro per la stampa del messaggio di vittoria
#define CUP1  "     _|_     "
#define CUP2  "     | |     "
#define CUP3  "|-----+-----|"
#define CUP4  "|    HAI    |"
#define CUP5  "|   VINTO   |"
#define CUP6  " '---------' "
#define CUP7  "  \\       /  "
#define CUP8  "   '.   .'   "
#define CUP9  "     | |     "
#define CUP10 "    .' '.    "
#define CUP11 "   _|___|_   "

#define CUP_ROWS 11
#define CUP_COLS 14

// Macro per la stampa del messaggio di sconfitta
#define FACE1 "    .--------.    "
#define FACE2 "  .'          '.  "
#define FACE3 " /   O      O   \\ "
#define FACE4 ":           `    :"
#define FACE5 "|                |"
#define FACE6 ":    .------.    :"
#define FACE7 " \\  '        '  / "
#define FACE8 "  '.          .'  "
#define FACE9 "    '-......-'    "
#define FACE10 "                  "
#define FACE11 "    HAI PERSO     "

#define FACE_ROWS 11
#define FACE_COLS 19

#define BACK_TO_MAIN_MENU "Ritorno al menu principale tra: %d"

// Dichiarazione delle funzioni e procedure utilizzate 
void mainGame(WINDOW* win, Point p, Difficulty difficultyMode);
void hudGame(WINDOW* win, Point p, int allyShipHealth, int nAliens);
void allyShipController(WINDOW* win, Point p, int pipeOut);
void enemyShipController(WINDOW* win, Point p, int pipeOut, int idNumber, Difficulty difficultyMode);
void bulletController(WINDOW* win, Point p, Point posShip, Direction direction, int pipeOut);
void bombController(WINDOW* win, Point p, Point posAlien, int pipeOut);
EndGame printObjects (WINDOW* win, Point p, int pipeIn, Difficulty difficultyMode);
EndGame isGameOver(int healthAllyShip, bool alienAllyCollision, int nAliensAlive);
void printStarShip (WINDOW* win, Point p, Object ship);
void printBullet (WINDOW* win, Object bullet);
void moveAllyShip (WINDOW* win, Point p, int* yPos, int* isBulletShot);
bool checkPos (Point p, int yPos, int size);
void endGame(pid_t aliens[], pid_t allyShip, Difficulty difficultyMode);
int getMaxAlien(Difficulty difficultyMode);
int getMaxHealth(Difficulty difficultyMode);
int getDelay(Difficulty difficultyMode);
void endGamePrint(WINDOW* win, Point p, EndGame gameStatus);