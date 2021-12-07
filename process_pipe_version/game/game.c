#include "game.h"

/**
 * @brief Procedura che gestisce il gioco
 * 
 * @param win 
 * @param p 
 */
void mainGame(WINDOW* win, Point p){
    pid_t pidEnemyShips[NUMBER_ENEMY_SHIPS], allyShip;
    int fileDes[DIM_PIPE];
    if(pipe(fileDes) == PROCESS_RETURN_FAILURE) {
        printExceptions(TYPE_EXCEPTION_PIPE_CREATION_FAILURE);
    }
    switch (allyShip = fork()) { //creazione processo navicella alleata
        case PROCESS_RETURN_FAILURE:
            printExceptions(TYPE_EXCEPTION_PROCESS_CREATION_FAILURE);
            break;
        case PROCESS_RETURN_CHILD_PID:
            close(fileDes[PIPE_READ]);
            allyShipController(win, p, fileDes[PIPE_WRITE]);
            usleep(100000);
            break;
        default:
            close(fileDes[PIPE_WRITE]);
            usleep(1000);
            printObjects(win, p, fileDes[PIPE_READ]);
            usleep(100000);
            break;
        }
    kill(allyShip, SIGTERM);
}

/**
 * @brief Procedura che visualizza la hud nella window
 * 
 * @param win 
 * @param p 
 */
void hudGame(WINDOW* win, Point p){

}
/**
 * @brief procedura che permette di dare un effetto grafico nello sfondo
 * 
 * @param win 
 * @param p 
 */
void mountainsBgEffect(WINDOW* win, Point p){

}

/**
 * @brief Procedura che gestisce la creazione e l'utilizzo della navicella alleata
 * 
 * @param pipeOut 
 */
void allyShipController(WINDOW* win, Point p, int pipeOut){
    int i;
    Ship ship;
    char sprite[ROWS_STARSHIP][COLS_STARSHIP] = STARSHIP;
    ship.pos.x = ALLY_BORDER_SPACE;
    ship.pos.y = Y_HSEPARATOR +  divideByTwo(p.y - Y_HSEPARATOR);
    for(i = 0; i<ROWS_STARSHIP; i++){
        strcpy(ship.sprite[i], sprite[i]);
    }
    while (true) {
        write(pipeOut, &ship, sizeof(ship));
        moveAllyShip(win, p, &ship.pos.y);
    }
}

/**
 * @brief Procedura che gestisce la creazione e lo spostamento della navicella nemica
 * 
 * @param pipeOut 
 */
void enemyShipController(WINDOW* win, Point p, int pipeOut){

}

/**
 * @brief Procedura che gestisce la generazione del proiettile da parte della navicella alleata
 * 
 * @param pipeOut 
 */
void bulletController(WINDOW* win, Point p, int pipeOut){
    
}

/**
 * @brief Procedura che gestisce la generazione della bomba da parte della navicella nemica
 * 
 * @param pipeOut 
 */
void bombController(WINDOW* win, Point p, int pipeOut){

}

/**
 * @brief Procedura che gestisce la visualizzazione degli oggetti nella finestra
 * 
 * @param pipeOut 
 */
void printObjects (WINDOW* win, Point p, int pipeIn) {
    Ship ship;
    while (true) {
        read(pipeIn, &ship, sizeof(ship));
        if(checkPos(p, ship.pos.y)){
            printStarShip(win, ship);
        }
        wrefresh(win);
    }
    
}

void checkCollision (WINDOW* win, Point p, int pipeIn) {

}
/**
 * @brief funzione che restituisce true se il gioco e' finito o meno 
 * 
 */
bool isGameOver (/*Pensare a cosa metterci*/){
    bool condizione;
    return condizione;
}

// sample of starship
void printStarShip (WINDOW* win, Ship ship) {
    int i,j, y;

    mvwprintw(win, ship.pos.y-OUTER_STARSHIP, ship.pos.x, BLANK_SPACE);
    mvwprintw(win, ship.pos.y+OUTER_STARSHIP, ship.pos.x, BLANK_SPACE);

    for(i=0;i<ROWS_STARSHIP;i++){
        for(j=0;j<COLS_STARSHIP;j++){
            y = ship.pos.y-divideByTwo(STARSHIP_SIZE) + i;
            mvwaddch(win, y, ship.pos.x+j, ship.sprite[i][j]);
        }
    }
}

void moveAllyShip (WINDOW* win, Point p, int* yPos) {
    keypad(win, TRUE);
    cbreak();
    int arrow = wgetch(win);
    keyPadSelector(win, Y_HSEPARATOR+STARSHIP_SIZE, p.y-STARSHIP_SIZE, arrow, yPos);
    nocbreak();
}

bool checkPos (Point p, int yPos) {
    return yPos > 0 && yPos < p.y-STARSHIP_SIZE;
}