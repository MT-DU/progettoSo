#include "utility.h"


/**
 * @brief Procedura che inizializza il tipo di oggetto e il tid degli elementi dell'array
 * 
 * @param array 
 * @param size 
 */
void objectArrayInitializer (Object array[], int size) {
    int i;
    for(i=0;i<size;i++){
        array[i].typeObject = UNDEFINED;
        array[i].tid = UNDEFINED_TID;
    }
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
        if(array[i].tid != UNDEFINED_TID){
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
            for(k=0;k<CLEAN_BULLET_AREA;k++){
                for(j=0;j<CLEAN_BULLET_AREA;j++){
                    mvwaddch(win, obj.pos.y-2+k, obj.pos.x-2+j, BLANK_SPACE);
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


/**
 * @brief Procedura che si occupa di stampare il messaggio di vittoria o sconfitta a fine partita
 * 
 * @param win 
 * @param p 
 * @param gameStatus 
 */
void endGamePrint(WINDOW* win, Point p, EndGame gameStatus) {
    wclear(win);
    int x = divideByTwo(p.x), y = divideByTwo(p.y);
    int i,j;
    char winPrint[CUP_ROWS][CUP_COLS];
    char defeatPrint[FACE_ROWS][FACE_COLS];

    switch(gameStatus){

        case VICTORY:

            for(i = 0; i < CUP_ROWS; i++){
                strcpy(winPrint[i], "");
                switch (i) {
                    case 0:
                        strcat(winPrint[i], CUP1);
                        break;
                    case 1:
                        strcat(winPrint[i], CUP2);
                        break;
                    case 2:
                        strcat(winPrint[i], CUP3);
                        break;
                    case 3:
                        strcat(winPrint[i], CUP4);
                        break;
                    case 4:
                        strcat(winPrint[i], CUP5);
                        break;
                    case 5:
                        strcat(winPrint[i], CUP6);
                        break;
                    case 6:
                        strcat(winPrint[i], CUP7);
                        break;
                    case 7:
                        strcat(winPrint[i], CUP8);
                        break;
                    case 8:
                        strcat(winPrint[i], CUP9);
                        break;
                    case 9:
                        strcat(winPrint[i], CUP10);
                        break;
                    case 10:
                        strcat(winPrint[i], CUP11);
                        break;
                }
            }

            for(i=0;i<CUP_ROWS;i++){
                mvwprintw(win, y+i-(CUP_COLS/2), x-(CUP_ROWS/2), winPrint[i]);
            }
            break;

        // defeat
        case DEFEAT:

            for(i = 0; i < FACE_ROWS; i++){
                strcpy(defeatPrint[i], "");
                switch (i) {
                    case 0:
                        strcat(defeatPrint[i], FACE1);
                        break;
                    case 1:
                        strcat(defeatPrint[i], FACE2);
                        break;
                    case 2:
                        strcat(defeatPrint[i], FACE3);
                        break;
                    case 3:
                        strcat(defeatPrint[i], FACE4);
                        break;
                    case 4:
                        strcat(defeatPrint[i], FACE5);
                        break;
                    case 5:
                        strcat(defeatPrint[i], FACE6);
                        break;
                    case 6:
                        strcat(defeatPrint[i], FACE7);
                        break;
                    case 7:
                        strcat(defeatPrint[i], FACE8);
                        break;
                    case 8:
                        strcat(defeatPrint[i], FACE9);
                        break;
                    case 9:
                        strcat(defeatPrint[i], FACE10);
                        break;
                    case 10:
                        strcat(defeatPrint[i], FACE11);
                        break;
                }
            }

            for(i=0;i<FACE_ROWS;i++){
                mvwprintw(win, y+i-(FACE_ROWS/2), x-(FACE_COLS/2), defeatPrint[i]);
            }
            break;
    }
    
    i = 5;
    while(i>0){
        mvwprintw(win, p.y-1, divideByTwo(p.x) - divideByTwo(sizeof(BACK_TO_MAIN_MENU)), BACK_TO_MAIN_MENU, i);
        wrefresh(win);
        usleep(1000000);
        i--;
    }
}