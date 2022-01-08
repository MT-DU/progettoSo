#include "../window/gestisciFinestre.h"
#include "../errors/errors.h"
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

/* MACRO PER GESTIRE I THREAD */
#define BUFFER_SIZE 1000
#define MAX_ENTITY 4
#define DEFAULT_VALUE 0
/* MACRO PER GESTIRE I MUTEX */

#define ROWS_STARSHIP 3
#define COLS_STARSHIP 6
#define OUTER_STARSHIP 2
#define STARSHIP_SIZE 3
#define ALLY_BORDER_SPACE 3
#define CLEAN_BULLET_AREA 5

#define ROWS_ALIEN 3
#define COLS_ALIEN 3
#define OUTER_ALIEN 1
#define ALIEN_SIZE 3
#define DISTANCE_FROM_CENTER_ALIEN 2

#define MOUNTAINS_ROWS 8
#define MOUNTAINS_COLS 21

#define MIN_RAND 0
#define MAX_RAND 100
#define BLANK_SPACE ' '

#define ENTITY_TYPES 5
#define MAX_ENTITY_ACTIVE 5
#define UNDEFINED_TID 0
#define OBJ_ALIVE 100
#define OBJ_DEAD 104

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
#define FACE10 "                  "
#define FACE11 "    HAI PERSO     "

#define FACE_ROWS 11
#define FACE_COLS 21

#define BACK_TO_MAIN_MENU "Ritorno al menu principale tra: %d"

typedef enum {
    ALLY_SHIP_TYPE, ENEMY_SHIP_TYPE, BULLET_TYPE, BOMB_TYPE, UNDEFINED
} TypeObject;

typedef enum {
    UP_DIRECTION, DOWN_DIRECTION
} Direction;

typedef enum {
    VICTORY, DEFEAT, CONTINUE
} EndGame;

typedef enum{
    EASY, HARD, CUSTOM
} Difficulty;

typedef enum{
    LOW = 1, MEDIUM, FULL
} Health;

typedef struct {
    Point pos;
    int tid;
    TypeObject typeObject;
    Direction direction;
    Health health;
    int idObj;
} Object;

void objectArrayInitializer (Object array[], int size);
bool addObject (Object array[], int size, Object newObj);
bool positionEquals(Point pos1, Point pos2);
void initializeArray(int array[], int size, int value);

bool checkBulletBombCollision (Point pos1, Point pos2);
bool checkAllyBombCollision(Point pos1, Point pos2);
bool checkBulletBombCollision (Point pos1, Point pos2);
bool checkAlienBulletCollision (Point pos1, Point pos2);
bool checkAllyAlienCollision (Point pos1);

int countObjects(Object array[], int size);

void clearObjects (WINDOW* win, Point p, Object obj);
void endGamePrint(WINDOW* win, Point p, EndGame gameStatus);