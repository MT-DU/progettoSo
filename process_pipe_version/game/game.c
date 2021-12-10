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
            usleep(100000);
            break;
        default:
            switch (printObject = fork()) {
            case PROCESS_RETURN_FAILURE:
                printExceptions(TYPE_EXCEPTION_PROCESS_CREATION_FAILURE);
                break;
            case PROCESS_RETURN_CHILD_PID: //processo figlio che gestisce la stampa
                close(fileDes[PIPE_WRITE]);
                close(fileDes[PIPE_READ]);
                close(fileDesPrint[PIPE_WRITE]);
                printObjects(win, p, fileDesPrint[PIPE_READ]);
                usleep(10000);
                break;
            default: //padre
                close(fileDes[PIPE_WRITE]);
                close(fileDesPrint[PIPE_READ]);
                checkCollision(win, p, fileDes[PIPE_READ], fileDesPrint[PIPE_WRITE]);
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
    Ship ship;
    char sprite[ROWS_STARSHIP][COLS_STARSHIP] = STARSHIP;
    ship.pos.x = ALLY_BORDER_SPACE;
    ship.pos.y = Y_HSEPARATOR + divideByTwo(p.y - Y_HSEPARATOR);
    ship.type = TYPE_ALLY;
    ship.direction = 0;
    bool isBulletShot = false;
    for(i = 0; i<ROWS_STARSHIP; i++){
        strcpy(ship.sprite[i], sprite[i]);
    }
    while (true) {
        write(pipeOut, &ship, sizeof(ship));
        moveAllyShip(win, p, &ship.pos.y);
        if(wgetch(win) == ASCII_CODE_SPACE_BAR){
            for(i = 0; i < NUMBER_BULLETS; i++){
                switch(bullets[i] = fork()){
                    case PROCESS_RETURN_FAILURE:
                        printExceptions(TYPE_EXCEPTION_PROCESS_CREATION_FAILURE);
                        break;
                    case PROCESS_RETURN_CHILD_PID: 
                        bulletController(win, p, ship.pos, i, pipeOut);
                        break;
                    default:
                        isBulletShot = true;
                        write(pipeOut, &isBulletShot, sizeof(bool));
                        write(pipeOut, &bullets[i], sizeof(pid_t));
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
    Bullet bullet;
    bullet.c = BULLET;
    bullet.pos.x = posShip.x + STARSHIP_SIZE;
    bullet.pos.y = direction == 0 ? posShip.y + 1 : posShip.y - 1;
    int cont = 0;
    while(cont < 10){
        switch (direction) {
        case SUPERIOR_BULLET:
            if(checkPos(p, bullet.pos.y)){
                bullet.pos.y--;
                bullet.pos.x++;
            }else{
                direction = INFERIOR_BULLET;
            }
            break;
        case INFERIOR_BULLET:
            if(checkPos(p, bullet.pos.y)){
                bullet.pos.y++;
                bullet.pos.x++;
            }else{
                direction = SUPERIOR_BULLET;
            }
        }
        
        write(pipeOut, &bullet, sizeof(Bullet));
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
    Ship ship;
    Bullet bullets[NUMBER_BULLETS];
    while (true) {
        read(pipeIn, &ship, sizeof(ship));
        read(pipeIn, &bullets[SUPERIOR_BULLET], sizeof(Bullet));
        read(pipeIn, &bullets[INFERIOR_BULLET], sizeof(Bullet));
        if(checkPos(p, ship.pos.y)){
            printStarShip(win, ship);
        }
        if(bullets[SUPERIOR_BULLET].c == BULLET){
            printBullet(win, bullets[SUPERIOR_BULLET]);
        }
        if(bullets[INFERIOR_BULLET].c == BULLET){
            printBullet(win, bullets[INFERIOR_BULLET]);
        }
        usleep(10000);
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
    Ship allyShip, genericShip;
    bool isCollisionDetected = false;
    bool allyShipWin = false, enemyShipWin = false, bulletShot = false;
    pid_t bullets[NUMBER_BULLETS] = {PROCESS_RETURN_CHILD_PID};
    Bullet bullet;
    do{
        read(pipeIn,&genericShip, sizeof(Ship));
        read(pipeIn, &bulletShot, sizeof(bool));
        if(bulletShot){
            read(pipeIn,&bullets[SUPERIOR_BULLET], sizeof(pid_t));
            read(pipeIn,&bullets[INFERIOR_BULLET], sizeof(pid_t));
        }
        
        if(genericShip.type == TYPE_ALLY){
            write(pipeOutPrint, &genericShip, sizeof(Ship));
        }
        if(bullets[SUPERIOR_BULLET] != PROCESS_RETURN_CHILD_PID){
            read(pipeIn, &bullet, sizeof(bullet));
            write(pipeOutPrint, &bullet, sizeof(Bullet));
        }
        if(bullets[INFERIOR_BULLET] != PROCESS_RETURN_CHILD_PID){
            read(pipeIn, &bullet, sizeof(bullet));
            write(pipeOutPrint, &bullet, sizeof(Bullet));
        }
    }while(true);
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

    mvwprintw(win, ship.pos.y-OUTER_STARSHIP, ship.pos.x, BLANK_SPACES);
    mvwprintw(win, ship.pos.y+OUTER_STARSHIP, ship.pos.x, BLANK_SPACES);

    for(i=0;i<ROWS_STARSHIP;i++){
        for(j=0;j<COLS_STARSHIP;j++){
            y = ship.pos.y-divideByTwo(STARSHIP_SIZE) + i;
            mvwaddch(win, y, ship.pos.x+j, ship.sprite[i][j]);
        }
    }
}
/**
 * @brief Stampa proiettili
 * 
 * @param win 
 * @param ship 
 */
void printBullet (WINDOW* win, Bullet bullet) {
    //TODO GESTIRE BOMBA
    int i,j, y, x;
    y = bullet.direction == SUPERIOR_BULLET ? bullet.pos.y + 1 : bullet.pos.y - 1;
    x = bullet.pos.x - 1; 
    mvwaddch(win, y, x, BLANK_SPACE);
    mvwaddch(win, bullet.pos.y, bullet.pos.x, bullet.c);
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