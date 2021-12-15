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

void entityArrayInitializer (Object array[ENTITY_TYPES][MAX_ENTITY_ACTIVE+1]) {
    
    int i, j = 0;
    for(i=0;i<ENTITY_TYPES;i++){
        array[i][j].nEntities = 0;
    }
}
void addEntity (Object array[ENTITY_TYPES][MAX_ENTITY_ACTIVE+1], Object newObj) {
    int i, j = 0;
    for(j=0;j<MAX_ENTITY_ACTIVE+1;j++){
        if((array[newObj.typeObject][j].nEntities == 0) && (j-1 >= 0)){  //incremento nEntities prendendo il numero progressivo dell'oggetto precedente
            array[newObj.typeObject][j].nEntities = array[newObj.typeObject][j-1].nEntities + 1;
        }else{  //se j-1 e' negativo, incremento nEntities di 1 (primo oggetto nella riga)
            array[newObj.typeObject][j].nEntities++;
        }
    }
}
