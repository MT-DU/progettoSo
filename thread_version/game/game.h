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
#define OBJ_ALIVE 100
#define OBJ_DEAD 104
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

#define FACE1 "    .--------.    "
#define FACE2 "  .'          '.  "
#define FACE3 " /   O      O   \\ "
#define FACE4 ":           `    :"
#define FACE5 "|                | "
#define FACE6 ":    .------.    : "
#define FACE7 " \\  '        '  / "
#define FACE8 "  '.          .'  "
#define FACE9 "    '-......-'    "
#define FACE10 "    HAI PERSO     "

#define FACE_ROWS 10
#define FACE_COLS 21
#define BACK_TO_MAIN_MENU "Ritorno al menu principale tra: %d"

void mainGame(WINDOW* win, Difficulty difficultyMode);
void hudGame(WINDOW* win, Point p, int allyShipHealth, int nAliens);
void* allyShipController();
void enemyShipController(WINDOW* win, Point p, int pipeOut, int idNumber, Difficulty difficultyMode);
void* bulletController();
void bombController(WINDOW* win, Point p, Point posAlien, int pipeOut, Difficulty difficultyMode);
void* printObjects ();
bool removeObject (Object array[], int size, pid_t pidObj);
EndGame isGameOver(int healthAllyShip, bool alienAllyCollision, int nAliensAlive);
void printStarShip (Object ship);
void moveAllyShip ();
void printBullet (Object bullet);
bool checkPos (Point p, int yPos, int size);
void checkWindowCollision(WINDOW* win, Point p, Object* obj);
void endGame(pid_t aliens[], pid_t allyShip, Difficulty difficultyMode);
int getMaxAlien(Difficulty difficultyMode);
int getMaxHealth(Difficulty difficultyMode);
int getDelay(Difficulty difficultyMode);
void endGamePrint(WINDOW* win, Point p, EndGame gameStatus);
int getMaxAlien(Difficulty difficultyMode);
void initializeBuffers();