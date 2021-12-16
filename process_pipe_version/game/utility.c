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
        array[i].pid = UNDEFINED_PID;
    }
}

bool addObject (Object array[], int size, Object newObj) {
    int i;
    if(array[size-1].pid==UNDEFINED_PID){ //se size-1 e' 0 allora non e' pieno
        for(i=0;i<size;i++){
            if(array[i].pid == UNDEFINED_PID){  //incremento nEntities prendendo il numero progressivo dell'oggetto precedente
                array[i] = newObj;
            }
        }
    }else{
        return false;
    }
    return true;
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

bool checkHitboxAlly(Point pos1, Point pos2){
    return (pos1.y-1 == pos2.y || pos1.y+1 == pos2.y || pos1.y==pos2.y) && (pos1.x == pos2.x);
}

bool checkBulletCollision (Point pos1, Point pos2){
    return (pos1.x == pos2.x  || pos1.x == pos2.x+1) && pos1.y == pos2.y;
}

bool checkAlienBulletCollision (Point pos1, Point pos2){
    return false; 
}