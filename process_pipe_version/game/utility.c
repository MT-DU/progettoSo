#include "utility.h"

/**
 * @brief Procedura che crea la pipe e gestisce l'eventuale errore generato
 * 
 * @param fileDes Descrittore della pipe da inizializzare
 */
void initializePipe(int fileDes[]){
    if(pipe(fileDes) == PROCESS_RETURN_FAILURE) {
        printExceptions(TYPE_EXCEPTION_PIPE_CREATION_FAILURE);
    }
}

/**
 * @brief Procedura che inizializza il tipo di oggetto e il pid degli elementi dell'array
 * 
 * @param array Vettore di oggetti da inizializzare
 * @param size Grandezza dell'array
 */
void objectArrayInitializer (Object array[], int size) {
    int i;
    for(i=0;i<size;i++){
        array[i].typeObject = UNDEFINED;
        array[i].pid = UNDEFINED_PID;
    }
}

/**
 * @brief Procedura che inizializza il valore intero degli elementi dell'array
 * 
 * @param array Vettore di interi da inizializzare
 * @param size Grandezza dell'array
 * @param value Valore da assegnare
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
 * @param pos1 Posizione della navicella alleata
 * @param pos2 Posizione della bomba
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
 * @param pos1 Posizione dell'alieno
 * @param pos2 Posizione del proiettile
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
 * @param pos1 Posizione della navicella nemica
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
 * @param array Array di oggetti da controllare
 * @param size Grandezza dell'array
 * @return int Numero di oggetti presenti
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
 * @brief Procedura che elimina dallo schermo lo sprite dell'oggetto nella sua ultima posizione
 * 
 * @param win Finestra su cui disegnare
 * @param p Struttura che contiene la risoluzione della finestra 
 * @param obj Oggetto da eliminare 
 */
void clearObjects (WINDOW* win, Point p, Object obj) {

    int i,j,k,y;

    switch(obj.typeObject){
        case BOMB_TYPE: // Bomba
            for(k=0;k<ROWS_ALIEN;k++){
                for(j=0;j<COLS_ALIEN;j++){
                    mvwaddch(win, obj.pos.y-1+k, obj.pos.x-1+j, BLANK_SPACE);
                }
            }
            break;
        case BULLET_TYPE: // Proiettile
            for(k=0;k<ROWS_ALIEN;k++){
                for(j=0;j<COLS_ALIEN;j++){
                    mvwaddch(win, obj.pos.y-1+k, obj.pos.x-1+j, BLANK_SPACE);
                }
            }               
            break;
        case ENEMY_SHIP_TYPE: // Navicella nemica
            for(k=0;k<ROWS_ALIEN;k++){
                for(j=0;j<COLS_ALIEN;j++){
                    y = obj.pos.y-divideByTwo(ALIEN_SIZE) + k;
                    mvwaddch(win, y, obj.pos.x+j, BLANK_SPACE);
                }
            }
            break;
    }
}