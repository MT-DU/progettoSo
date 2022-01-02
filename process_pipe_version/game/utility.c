#include "utility.h"

void initializePipe(int fileDes[]){
    if(pipe(fileDes) == PROCESS_RETURN_FAILURE) {
        printExceptions(TYPE_EXCEPTION_PIPE_CREATION_FAILURE);
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

void initializeArray(int array[], int size, int value){
    int i;
    for(i = 0; i < size; i++){
        array[i] = value;
    }
}

bool checkAllyBombCollision(Point pos1, Point pos2){
    pos1.x = pos1.x + STARSHIP_SIZE;
    return (pos1.y-1 == pos2.y || pos1.y+1 == pos2.y || pos1.y==pos2.y) && (pos1.x == pos2.x || pos1.x+1 == pos2.x);
}

bool checkBulletBombCollision (Point pos1, Point pos2){
    return (pos1.x == pos2.x  || pos1.x == pos2.x+1) && pos1.y == pos2.y;
}

bool checkAlienBulletCollision (Point pos1, Point pos2){
    pos1.x = pos1.x+1;
    return (pos1.x-1 == pos2.x || pos1.x == pos2.x || pos1.x+1 == pos2.x) && (pos1.y-1 == pos2.y || pos1.y == pos2.y || pos1.y+1 == pos2.y);
}

bool checkAllyAlienCollision (Point pos1) {
    int i,j;
    bool check = pos1.x == (COLS_STARSHIP + COLS_STARSHIP);
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
    int i = 0, contAliens = 0;
    for(i = 0; i<size; i++){
        if(array[i].pid != UNDEFINED_PID){
            contAliens++;
        }
    }
    return contAliens;
}

void clearObjects (WINDOW* win, Point p, Object obj) {

    int i,j,k,y;

    switch(obj.typeObject){
        case BOMB_TYPE:
            for(k=0;k<ROWS_ALIEN;k++){
                for(j=0;j<COLS_ALIEN;j++){
                    mvwaddch(win, obj.pos.y-1+k, obj.pos.x-1+j, BLANK_SPACE);
                }
            }
            break;
        case BULLET_TYPE:
            for(k=0;k<ROWS_ALIEN;k++){
                for(j=0;j<COLS_ALIEN;j++){
                    mvwaddch(win, obj.pos.y-1+k, obj.pos.x-1+j, BLANK_SPACE);
                }
            }               
            break;
        case ENEMY_SHIP_TYPE:
            for(k=0;k<ROWS_ALIEN;k++){
                for(j=0;j<COLS_ALIEN;j++){
                    y = obj.pos.y-divideByTwo(ALIEN_SIZE) + k;
                    mvwaddch(win, y, obj.pos.x+j, BLANK_SPACE);
                }
            }
            break;

    }


}