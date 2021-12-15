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

typedef enum {
    ALLY_SHIP_TYPE, ENEMY_SHIP_TYPE, BIG_ENEMY_SHIP_TYPE, BULLET_TYPE, BOMB_TYPE
} TypeObject;

#define ENTITY_TYPES 5
#define MAX_ENTITY_ACTIVE 5

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
void entityArrayInitializer (Object array[ENTITY_TYPES][MAX_ENTITY_ACTIVE+1]);
void addEntity (Object array[ENTITY_TYPES][MAX_ENTITY_ACTIVE+1], Object newObj);