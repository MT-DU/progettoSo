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
                close(fileDes[PIPE_READ]);
                close(fileDesPrint[PIPE_WRITE]);
                printObjects(win, p, fileDesPrint[PIPE_READ]);
                break;
            default: // padre
                close(fileDes[PIPE_WRITE]);
                close(fileDesPrint[PIPE_READ]);
                checkCollision(win, p, fileDes[PIPE_READ], fileDesPrint[PIPE_WRITE]);
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

    int x = 0, y = getmaxy(win)-MOUNTAINS_ROWS, i = -1, j = 0, k;

    char mountains[MOUNTAINS_ROWS][MOUNTAINS_COLS]={
    {"        /\\    *      "},
    {"       /**\\       *  "},
    {"   *  /****\\         "},
    {"     /*/\\***\\        "},
    {"    / /**\\   \\  /\\   "},
    {"   / /    \\   \\/YY\\  "},
    {" /\\ /YYYYYY\\   \\YYY\\ "},
    {"/  \\YYYYYYYY\\YYY\\YYY\\"}};

    while(true){
        
        if(x > getmaxx(win)-10){
            for(k=0;k<MOUNTAINS_ROWS;k++){
                move(y+k,1);
                delch();
            }
        }
        
        
        if(i == MOUNTAINS_COLS-1){
            i=0;
        } else {
            i++;
        }

        
        for(j=0;j<MOUNTAINS_ROWS;j++){
            mvaddch(y+j, x, mountains[j][i]);
        }

        usleep(200000);
    }
    x++;

}

/**
 * @brief Procedura che gestisce la creazione e l'utilizzo della navicella alleata
 * 
 * @param pipeOut 
 */
void allyShipController(WINDOW* win, Point p, int pipeOut){
    pid_t bullets[NUMBER_BULLETS];
    int i = 0;
    Object ship; //WARNING il pid della struttura ship non e' inizializzato
    ship.pos.x = ALLY_BORDER_SPACE;
    ship.pos.y = Y_HSEPARATOR + divideByTwo(p.y - Y_HSEPARATOR);
    ship.typeObject = ALLY_SHIP_TYPE;
    ship.direction = 0;
    ship.pid = PROCESS_RETURN_CHILD_PID;
    int nBulletsActive = 0;
    int isBulletShot = false; //TODO da fixare la condizione

    while (true) {
        write(pipeOut, &ship, sizeof(Object));
        moveAllyShip(win, p, &ship.pos.y, &isBulletShot);
        if(isBulletShot && nBulletsActive < 5){
            isBulletShot = false;
            nBulletsActive++;
            switch(bullets[UP_DIRECTION] = fork()){
                case PROCESS_RETURN_FAILURE:
                    printExceptions(TYPE_EXCEPTION_PROCESS_CREATION_FAILURE);
                    break;
                case PROCESS_RETURN_CHILD_PID:
                    bulletController(win, p, ship.pos, UP_DIRECTION, pipeOut, &nBulletsActive);
                    break;
                default:
                    switch(bullets[DOWN_DIRECTION] = fork()){
                        case PROCESS_RETURN_FAILURE:
                            printExceptions(TYPE_EXCEPTION_PROCESS_CREATION_FAILURE);
                            break;
                        case PROCESS_RETURN_CHILD_PID:
                            bulletController(win, p, ship.pos, DOWN_DIRECTION, pipeOut, &nBulletsActive);
                            break;
                        default:
                           break;
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
void bulletController(WINDOW* win, Point p, Point posShip, Direction direction, int pipeOut, int* nBulletsActive){
    Object bullet;
    bullet.pos.x = posShip.x + COLS_STARSHIP;
    bullet.pos.y = posShip.y;
    bullet.typeObject = BULLET_TYPE;
    bullet.direction = direction;
    while(p.x >= bullet.pos.x){
        switch (bullet.direction) {
        case UP_DIRECTION:
            if(checkPos(p, bullet.pos.y)){
                bullet.pos.y--;
                bullet.pos.x++;
            } else {
                bullet.direction = DOWN_DIRECTION;
                bullet.pos.y++;
            }
            break;
        case DOWN_DIRECTION:
            if(checkPos(p, bullet.pos.y)){
                bullet.pos.y++;
                bullet.pos.x++;
            } else {
                bullet.direction = UP_DIRECTION;
                bullet.pos.y--;
            }
        }
        write(pipeOut, &bullet, sizeof(Object));
        usleep(1000000);
    }
    char str[10];
    sprintf(str, "proiettili attivi: %d", *nBulletsActive);
    perror(str);
    kill(getpid(), SIGUSR1);
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
    Object obj;

    while (true) {
        if(read(pipeIn, &obj, sizeof(Object)) < 0) {
            perror("Nothing to read");
        } else {
            switch(obj.typeObject){
                case ALLY_SHIP_TYPE:
                    printStarShip(win, obj);
                    break;
                case ENEMY_SHIP_TYPE:
                    //TODO
                    break;
                case BIG_ENEMY_SHIP_TYPE:
                    //TODO
                    break;
                case BULLET_TYPE:
                    //usleep(100000);
                    printBullet(win, obj);
                case BOMB_TYPE:
                    printBullet(win, obj);
                    break;
            }
        }
        //mountainsBgEffect(win, p);
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
    Object allyShip, obj;
    bool isCollisionDetected = false;
    bool allyShipWin = false, enemyShipWin = false, bulletShot = false;
    pid_t bullets[NUMBER_BULLETS] = {PROCESS_RETURN_CHILD_PID};
    Object bullet;
    do{
        read(pipeIn, &obj, sizeof(Object));
        switch(obj.typeObject){
            case ALLY_SHIP_TYPE:
                write(pipeOutPrint, &obj, sizeof(Object));
                break;
            case BULLET_TYPE:
                write(pipeOutPrint, &obj, sizeof(Object));
                break;
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
    return yPos > Y_HSEPARATOR && yPos < p.y-STARSHIP_SIZE;
}