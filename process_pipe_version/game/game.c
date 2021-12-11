#include "game.h"

/**
 * @brief Procedura che gestisce il gioco
 * 
 * @param win 
 * @param p 
 */
void mainGame(WINDOW* win, Point p){
    pid_t pidEnemyShips[NUMBER_ENEMY_SHIPS], allyShip, printObject;
    int fileDes[DIM_PIPE];
    int fileDesPrint[DIM_PIPE];
    if(pipe(fileDes) == PROCESS_RETURN_FAILURE) {
        printExceptions(TYPE_EXCEPTION_PIPE_CREATION_FAILURE);
    }
    if(pipe(fileDesPrint) == PROCESS_RETURN_FAILURE) {
        printExceptions(TYPE_EXCEPTION_PIPE_CREATION_FAILURE);
    }
    switch (allyShip = fork()) { //creazione processo navicella alleata
        case PROCESS_RETURN_FAILURE:
            printExceptions(TYPE_EXCEPTION_PROCESS_CREATION_FAILURE);
            break;
        case PROCESS_RETURN_CHILD_PID: //processo figlio che gestisce la navicella alleata
            close(fileDesPrint[PIPE_WRITE]);
            close(fileDesPrint[PIPE_READ]);
            close(fileDes[PIPE_READ]);
            allyShipController(win, p, fileDes[PIPE_WRITE]);
            break;
        default:
            switch (printObject = fork()) {
            case PROCESS_RETURN_FAILURE:
                printExceptions(TYPE_EXCEPTION_PROCESS_CREATION_FAILURE);
                break;
            case PROCESS_RETURN_CHILD_PID: //processo figlio che gestisce la stampa
                close(fileDes[PIPE_WRITE]);
                //close(fileDes[PIPE_READ]);
                close(fileDesPrint[PIPE_WRITE]);
                close(fileDesPrint[PIPE_READ]);
                printObjects(win, p, fileDes[PIPE_READ]);
                break;
            default: // padre
                //close(fileDes[PIPE_WRITE]);
                //close(fileDesPrint[PIPE_READ]);
                //checkCollision(win, p, fileDes[PIPE_READ], fileDesPrint[PIPE_WRITE]);
                wait(0);
                break;
            }
        }
    kill(allyShip, SIGTERM);
    kill(printObject, SIGTERM);
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
    pid_t bullets[NUMBER_BULLETS];
    int i;
    Object ship; //WARNING il pid della struttura ship non e' inizializzato
    ship.pos.x = ALLY_BORDER_SPACE;
    ship.pos.y = Y_HSEPARATOR + divideByTwo(p.y - Y_HSEPARATOR);
    ship.typeObject = ALLY_SHIP_TYPE;
    ship.direction = 0;
    int isBulletShot; //TODO da fixare la condizione

    while (true) {
        write(pipeOut, &ship, sizeof(Object));
        moveAllyShip(win, p, &ship.pos.y, &isBulletShot);
        if(isBulletShot){
            for(i = 0; i < NUMBER_BULLETS; i++){
                switch(bullets[i] = fork()){
                    case PROCESS_RETURN_FAILURE:
                        printExceptions(TYPE_EXCEPTION_PROCESS_CREATION_FAILURE);
                        break;
                    case PROCESS_RETURN_CHILD_PID: 
                        bulletController(win, p, ship.pos, i, pipeOut);
                        //TODO aggiungere alla funzione bullet il pid in modo da poterlo ammazzare con le collisioni
                        break;
                    default:
                        //isBulletShot = true;
                        //write(pipeOut, &isBulletShot, sizeof(bool));
                        //write(pipeOut, &bullets[i], sizeof(pid_t));
                }
            }
        }
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
void bulletController(WINDOW* win, Point p, Point posShip, int direction, int pipeOut){
    Object bullet;
    bullet.pos.x = posShip.x + STARSHIP_SIZE;
    bullet.pos.y = direction == UP_DIRECTION ? posShip.y + 1 : posShip.y - 1;
    int cont = 0;
    while(true){
        switch (direction) {
        case UP_DIRECTION:
            if(checkPos(p, bullet.pos.y)){
                bullet.pos.y--;
                bullet.pos.x++;
            } else {
                direction = UP_DIRECTION;
            }
            break;
        case DOWN_DIRECTION:
            if(checkPos(p, bullet.pos.y)){
                bullet.pos.y++;
                bullet.pos.x++;
            } else {
                direction = DOWN_DIRECTION;
            }
        }
        write(pipeOut, &bullet, sizeof(Object));
        cont++;
    }
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
    Object unknown;

    while (true) {
        read(pipeIn, &unknown, sizeof(Object));
        switch(unknown.typeObject){
            case ALLY_SHIP_TYPE:
                printStarShip(win, unknown);
                break;
            case ENEMY_SHIP_TYPE:
                //TODO
                break;
            case BIG_ENEMY_SHIP_TYPE:
                //TODO
                break;
            case BULLET_TYPE:
                printBullet(win, unknown);
            case BOMB_TYPE:
                //TODO
                break;
        }
        wrefresh(win);
    }
}

/**
 * @brief Lo scopo di questa procedura è quella di scrivere all'interno della pipe se 
 * 
 * @param win 
 * @param p 
 * @param pipeIn 
 */
void checkCollision (WINDOW* win, Point p, int pipeIn, int pipeOutPrint) {
    Object allyShip, genericShip;
    bool isCollisionDetected = false;
    bool allyShipWin = false, enemyShipWin = false, bulletShot = false;
    pid_t bullets[NUMBER_BULLETS] = {PROCESS_RETURN_CHILD_PID};
    Object bullet;
    do{
        read(pipeIn,&genericShip, sizeof(Object));
        read(pipeIn, &bulletShot, sizeof(bool));
        if(bulletShot){
            read(pipeIn,&bullets[UP_DIRECTION], sizeof(pid_t));
            read(pipeIn,&bullets[DOWN_DIRECTION], sizeof(pid_t));
        }
        
        if(genericShip.typeObject == ALLY_SHIP_TYPE){
            write(pipeOutPrint, &genericShip, sizeof(Object));
        }
        if(bullets[UP_DIRECTION] != PROCESS_RETURN_CHILD_PID){
            read(pipeIn, &bullet, sizeof(Object));
            write(pipeOutPrint, &bullet, sizeof(Object));
        }
        if(bullets[DOWN_DIRECTION] != PROCESS_RETURN_CHILD_PID){
            read(pipeIn, &bullet, sizeof(Object));
            write(pipeOutPrint, &bullet, sizeof(Object));
        }
    } while(true);
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
void printStarShip (WINDOW* win, Object ship) {
    int i,j, y;

    mvwprintw(win, ship.pos.y-OUTER_STARSHIP, ship.pos.x, BLANK_SPACES);
    mvwprintw(win, ship.pos.y+OUTER_STARSHIP, ship.pos.x, BLANK_SPACES);

    char sprite[ROWS_STARSHIP][COLS_STARSHIP] = STARSHIP;

    for(i=0;i<ROWS_STARSHIP;i++){
        for(j=0;j<COLS_STARSHIP;j++){
            y = ship.pos.y-divideByTwo(STARSHIP_SIZE) + i;
            mvwaddch(win, y, ship.pos.x+j, sprite[i][j]);
        }
    }
}
/**
 * @brief Stampa proiettili
 * 
 * @param win 
 * @param ship 
 */
void printBullet (WINDOW* win, Object bullet) {
    int y, x;

    switch (bullet.typeObject) {
        case BULLET_TYPE:
            y = bullet.direction == UP_DIRECTION ? bullet.pos.y + 1 : bullet.pos.y - 1;
            x = bullet.pos.x - 1; 
            mvwaddch(win, y, x, BLANK_SPACE);
            mvwaddch(win, bullet.pos.y, bullet.pos.x, BULLET_SPRITE);
            break;
        case BOMB_TYPE:
            y = bullet.pos.y;
            x = bullet.pos.x + 1;
            mvwaddch(win, y, x, BLANK_SPACE);
            mvwaddch(win, bullet.pos.y, bullet.pos.x, BOMB_SPRITE);
            break;
    }
}
    

void moveAllyShip (WINDOW* win, Point p, int* yPos, int* isBulletShot) {
    keypad(win, TRUE);
    cbreak();
    int arrow = wgetch(win);
    if(arrow == ASCII_CODE_SPACE_BAR) {
        *isBulletShot = 1;
    }
    keyPadSelector(win, Y_HSEPARATOR+STARSHIP_SIZE, p.y-STARSHIP_SIZE, arrow, yPos);
    nocbreak();
}

bool checkPos (Point p, int yPos) {
    return yPos > 0 && yPos < p.y-STARSHIP_SIZE;
}