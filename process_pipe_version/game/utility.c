#include "utility.h"

void initializePipe(int fileDes[]){
    if(pipe(fileDes) == PROCESS_RETURN_FAILURE) {
        printExceptions(TYPE_EXCEPTION_PIPE_CREATION_FAILURE);
    }
}

/**
 * @brief Procedura che cambia la direzione dell'alieno al verificarsi di una collisione
 * 
 * @param status 
 * @param direction 
 */
void changeDirection(Status status, Direction* direction){
    switch (status.collision) {
        case BOUNCE_COLLISION:
            if((*direction) == UP_DIRECTION){
                (*direction) = DOWN_DIRECTION;
            } else {
                (*direction) = UP_DIRECTION;
            }
        break;
    }
}

void objectArrayInitializer (Object array[], int size) {
    int i;
    for(i=0;i<size;i++){
        array[i].typeObject = UNDEFINED;
        array[i].pid = UNDEFINED_PID;
    }
}

bool positionEquals(Point pos1, Point pos2){
    return pos1.y == pos2.y && pos1.x == pos2.x;
}

bool checkAllyBombCollision(Point pos1, Point pos2){
    pos1.x = pos1.x + STARSHIP_SIZE;
    return (pos1.y-1 == pos2.y || pos1.y+1 == pos2.y || pos1.y==pos2.y) && (pos1.x == pos2.x || pos1.x+1 == pos2.x);
}

bool checkBulletBombCollision (Point pos1, Point pos2){
    return (pos1.x == pos2.x  || pos1.x == pos2.x+1) && pos1.y == pos2.y;
}

bool checkAlienBulletCollision (Point pos1, Point pos2){
    return (pos1.x-1 == pos2.x || pos1.x == pos2.x || pos1.x+1 == pos2.x) && (pos1.y-1 == pos2.y || pos1.y == pos2.y || pos1.y+1 == pos2.y);
}

bool checkAllyAlienCollision (Point pos1, Point pos2) {
    int i,j;
    bool check = false;

    for (i=0;i<3;i++) {
        for(j=0;j<3;j++) {
            if(pos1.y-1+i == pos2.y-1+j && (pos1.x+STARSHIP_SIZE == pos2.x-1 || pos1.x+STARSHIP_SIZE == pos2.x-2)){
                check = true;
            }
        }
    }
    return check;
}

bool checkObjOutOfScreenLeft (Point pos, int spriteSize) {
    return pos.x < 0 + spriteSize;
}

bool checkObjOutOfScreenRight (Point res, Point pos, int spriteSize){
    return pos.x > res.x - spriteSize;
}

bool checkObjOutOfScreenUpDown (Point res, Point pos, int spriteSize){
    return pos.y < 0 + spriteSize || pos.y > res.y - spriteSize;
}

int countObjects(Object array[], int size){
    int i = 0;
    while (i<size && array[i].pid != UNDEFINED){
        i++;
    }
    return i;
}