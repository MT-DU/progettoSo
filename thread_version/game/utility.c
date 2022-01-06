#include "utility.h"


/**
 * @brief Procedura che inizializza il tipo di oggetto e il pid degli elementi dell'array
 * 
 * @param array 
 * @param size 
 */
void objectArrayInitializer (Object array[], int size) {
    int i;
    for(i=0;i<size;i++){
        array[i].typeObject = UNDEFINED;
        array[i].pid = UNDEFINED_PID;
    }
}

/**
 * @brief Funzione che verifica se due posizioni sono uguali
 * 
 * @param pos1 
 * @param pos2 
 * @return true se sono uguali
 * @return false se sono diversi
 */
bool positionEquals(Point pos1, Point pos2){
    return pos1.y == pos2.y && pos1.x == pos2.x;
}

/**
 * @brief Procedura che popola un array di interi dato un value
 * 
 * @param array 
 * @param size 
 * @param value
 */
void initializeArray(int array[], int size, int value){
    int i;
    for(i = 0; i < size; i++){
        array[i] = value;
    }
}

/**
 * @brief Funzione per il check delle collisioni tra navicella alleata e bomba
 * 
 * @param pos1 allyShip
 * @param pos2 bomba
 * @return true se e' avvenuta la collisione
 * @return false se non e' avvenuta la collisione
 */
bool checkAllyBombCollision(Point pos1, Point pos2){
    pos1.x = pos1.x + STARSHIP_SIZE;
    return (pos1.y-1 == pos2.y || pos1.y+1 == pos2.y || pos1.y==pos2.y) && (pos1.x == pos2.x || pos1.x+1 == pos2.x);
}

/**
 * @brief Funzione per il check delle collisioni tra proiettile e bomba
 * 
 * @param pos1 
 * @param pos2 
 * @return true 
 * @return false 
 */
bool checkBulletBombCollision (Point pos1, Point pos2){
    return (pos1.x == pos2.x  || pos1.x == pos2.x+1) && pos1.y == pos2.y;
}

/**
 * @brief Funzione per il check delle collisioni tra alieno e proiettile
 * 
 * @param pos1 
 * @param pos2 
 * @return true se e' avvenuta la collisione
 * @return false se non e' avvenuta la collisione
 */
bool checkAlienBulletCollision (Point pos1, Point pos2){
    pos1.x = pos1.x+1;
    return 
    (pos1.x-2 == pos2.x && pos1.y-1 == pos2.y)||
    (pos1.x-2 == pos2.x && pos1.y == pos2.y)||
    (pos1.x-2 == pos2.x && pos1.y+1 == pos2.y) ||

    (pos1.x-1 == pos2.x && pos1.y-1 == pos2.y) ||
    (pos1.x-1 == pos2.x && pos1.y == pos2.y)||
    (pos1.x-1 == pos2.x && pos1.y+1 == pos2.y)||

    (pos1.x == pos2.x && pos1.y-1 == pos2.y)||
    (pos1.x == pos2.x && pos1.y+1 == pos2.y)||

    (pos1.x+1 == pos2.x && pos1.y-1 == pos2.y)||
    (pos1.x+1 == pos2.x && pos1.y == pos2.y)||
    (pos1.x+1 == pos2.x && pos1.y+1 == pos2.y) ||

    (pos1.x+2 == pos2.x && pos1.y-1 == pos2.y)||
    (pos1.x+2 == pos2.x && pos1.y == pos2.y)||
    (pos1.x+2 == pos2.x && pos1.y+1 == pos2.y);
}

/**
 * @brief Funzione per il check delle collisioni tra navicella alleata e alieno
 * 
 * @param pos1 alieno
 * @return true se e' avvenuta la collisione
 * @return false se non e' avvenuta la collisione
 */
bool checkAllyAlienCollision (Point pos1) {
    int i,j;
    bool check = pos1.x == (COLS_STARSHIP + COLS_STARSHIP);
    return check;
}

/**
 * @brief Funzione che restituisce il numero di oggetti presenti all'interno dell'array
 * 
 * @param array 
 * @param size 
 * @return int 
 */
int countObjects(Object array[], int size){
    int i = 0, cont = 0;
    for(i = 0; i<size; i++){
        if(array[i].pid != UNDEFINED_PID){
            cont++;
        }
    }
    return cont;
}

/**
 * @brief Procedura che pulisce l'ultima posizione dell'oggetto
 * 
 * @param win 
 * @param p 
 * @param obj 
 */
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