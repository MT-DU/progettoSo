#include "../window/gestisciFinestre.h"
#include "../errors/errors.h"
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

/* MACRO PER GESTIRE I THREAD */
#define BUFFER_SIZE 1000

/* MACRO PER GESTIRE I MUTEX */

#define ROWS_STARSHIP 3
#define COLS_STARSHIP 6
#define OUTER_STARSHIP 2
#define STARSHIP_SIZE 3
#define ALLY_BORDER_SPACE 3

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

typedef enum {
    ALLY_SHIP_TYPE, ENEMY_SHIP_TYPE, BIG_ENEMY_SHIP_TYPE, BULLET_TYPE, BOMB_TYPE, UNDEFINED
} TypeObject;

#define ENTITY_TYPES 5
#define MAX_ENTITY_ACTIVE 5
#define UNDEFINED_PID -2
typedef enum {
    UP_DIRECTION, DOWN_DIRECTION
} Direction;

typedef enum {
    VICTORY, DEFEAT, CONTINUE
} EndGame;

typedef enum{
    EASY, HARD, CUSTOM
} Difficulty;

typedef struct {
    Point pos;
    pid_t pid;
    TypeObject typeObject;
    Direction direction;
    int health;
    int idObj;
} Object;

void objectArrayInitializer (Object array[], int size);
bool addObject (Object array[], int size, Object newObj);
bool removeObject (Object array[], int size, pid_t pidObj);
bool positionEquals(Point pos1, Point pos2);
void initializeArray(int array[], int size, int value);

bool checkBulletBombCollision (Point pos1, Point pos2);
bool checkAllyBombCollision(Point pos1, Point pos2);
bool checkBulletBombCollision (Point pos1, Point pos2);
bool checkAlienBulletCollision (Point pos1, Point pos2);
bool checkAllyAlienCollision (Point pos1);

int countObjects(Object array[], int size);

void clearObjects (WINDOW* win, Point p, Object obj);
