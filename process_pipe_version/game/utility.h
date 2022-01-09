#include "../window/gestisciFinestre.h"
#include "../errors/errors.h"
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

// Macro per la gestione del valore di ritorno della fork
#define PROCESS_RETURN_FAILURE -1
#define PROCESS_RETURN_CHILD_PID 0

// Macro per la gestione delle pipe
#define PIPE_READ 0
#define PIPE_WRITE 1

// Macro relative allo sprite della navicella alleata 
#define ROWS_STARSHIP 3
#define COLS_STARSHIP 6
#define OUTER_STARSHIP 2
#define STARSHIP_SIZE 3
#define ALLY_BORDER_SPACE 3

// Macro relative allo sprite della navicella nemica
#define ROWS_ALIEN 3
#define COLS_ALIEN 3
#define OUTER_ALIEN 1
#define ALIEN_SIZE 3
#define DISTANCE_FROM_CENTER_ALIEN 2

// Macro generiche
#define MIN_RAND 0
#define MAX_RAND 100
#define BLANK_SPACE ' '
#define ENTITY_TYPES 5
#define MAX_ENTITY_ACTIVE 5
#define UNDEFINED_PID -2
#define MIN_ALIENS 1
#define MAX_ALIENS 12

// Dichiarazione delle enumerazioni e delle struct utilizzate

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
    int numAliens;
    Difficulty type;
} DifficultyType;

typedef struct {
    Point pos;
    pid_t pid;
    TypeObject typeObject;
    Direction direction;
    Health health;
    int idObj;
} Object;

// Dichiarazione delle funzioni e procedure utilizzate 
void initializePipe(int fileDes[]);
void objectArrayInitializer (Object array[], int size);
void initializeArray(int array[], int size, int value);
bool checkAllyBombCollision(Point pos1, Point pos2);
bool checkAlienBulletCollision (Point pos1, Point pos2);
bool checkAllyAlienCollision (Point pos1);
int countObjects(Object array[], int size);
void clearObjects (WINDOW* win, Point p, Object obj);
