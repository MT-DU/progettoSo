#include "utility.h"

void initializePipe(int fileDes[]){
    if(pipe(fileDes) == PROCESS_RETURN_FAILURE) {
        printExceptions(TYPE_EXCEPTION_PIPE_CREATION_FAILURE);
    }
}
void initializeNonBlockingPipe(int fileDes[]){
    initializePipe(fileDes);
    fcntl(fileDes[PIPE_WRITE], F_SETFL, O_NDELAY);
    fcntl(fileDes[PIPE_READ], F_SETFL, O_NDELAY);
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

bool addObject (Object array[], int size, Object newObj) {
    int i;
    if(array[size-1].pid==UNDEFINED_PID){ //se size-1 e' 0 allora non e' pieno
        for(i=0;i<size;i++){
            if(array[i].pid == UNDEFINED_PID || array[i].pid == newObj.pid){
                array[i] = newObj;
                return true;
            }
        }
    }else{
        return false;
    }
}

bool removeObject (Object array[], int size, pid_t pidObj) {
    int i, j;
    for(i=0;i<size;i++){
        if(array[i].pid == pidObj){  //incremento nEntities prendendo il numero progressivo dell'oggetto precedente
            for(j = i; j<size-1; j++){
                array[j] = array[j+1];
            }
            return true;
        }
    }
    return false;
}

void catchZombies(Object array[], int size, pid_t pidObj){
    int i, status;
    for(i = 0; i<size; i++){
        waitpid(pidObj, &status, WNOHANG);
        if(WIFEXITED(status)){
            removeObject(array, size, pidObj);
        }
    }
}

bool positionEquals(Point pos1, Point pos2){
    return pos1.y == pos2.y && pos1.x == pos2.x;
}

bool checkAllyBombCollision(Point pos1, Point pos2){
    pos1.x = pos1.x + STARSHIP_SIZE;
    return (pos1.y-1 == pos2.y || pos1.y+1 == pos2.y || pos1.y==pos2.y) && (pos1.x == pos2.x);
}

bool checkBulletBombCollision (Point pos1, Point pos2){
    return (pos1.x == pos2.x  || pos1.x == pos2.x+1) && pos1.y == pos2.y;
}

bool checkAlienBulletCollision (Point pos1, Point pos2){
    return (pos1.x-1 == pos2.x && (pos1.y == pos2.y || pos1.y == pos2.y-1 || pos1.y == pos2.y+1));
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