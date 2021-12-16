#include "../window/gestisciFinestre.h"
#include "../errors/errors.h"
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <fcntl.h>

/* MACRO PER GESTIRE I PROCESSI */
#define PROCESS_RETURN_FAILURE -1
#define PROCESS_RETURN_CHILD_PID 0

/* MACRO PER GESTIRE LE PIPE */
#define PIPE_READ 0
#define PIPE_WRITE 1

#define ROWS_STARSHIP 3
#define COLS_STARSHIP 6
#define OUTER_STARSHIP 2
#define STARSHIP_SIZE 3

#define ROWS_ALIEN 3
#define COLS_ALIEN 3
#define OUTER_ALIEN 1
#define ALIEN_SIZE 3

#define MOUNTAINS_ROWS 8
#define MOUNTAINS_COLS 21

#define MIN_RAND 0
#define MAX_RAND 100

typedef enum {
    ALLY_SHIP_TYPE, ENEMY_SHIP_TYPE, BIG_ENEMY_SHIP_TYPE, BULLET_TYPE, BOMB_TYPE
} TypeObject;

#define ENTITY_TYPES 5
#define MAX_ENTITY_ACTIVE 5
#define UNDEFINED_PID -2
typedef enum {
    UP_DIRECTION, DOWN_DIRECTION
} Direction;

typedef enum {
    NO_COLLISION, BOUNCE_COLLISION, DEATH_COLLISION
} CollisionType;

typedef struct {
    CollisionType collision;
} Status;

typedef struct {
    Point pos;
    pid_t pid;
    TypeObject typeObject;
    Direction direction;
    Status status;
    int nEntities;
    int health;
} Object;

void initializePipe(int fileDes[]);
void initializeNonBlockingPipe(int fileDes[]);
void changeDirection(Status status, Direction* direction);
void objectArrayInitializer (Object array[], int size);
bool addObject (Object array[], int size, Object newObj);
bool removeObject (Object array[], int size, pid_t pidObj);
void catchZombies(Object array[], int size, pid_t pidObj);
bool positionEquals(Point pos1, Point pos2);
bool checkBulletCollision (Point pos1, Point pos2);
bool checkHitboxAlly(Point pos1, Point pos2);
bool checkBulletCollision (Point pos1, Point pos2);
bool checkAlienBulletCollision (Point pos1, Point pos2);