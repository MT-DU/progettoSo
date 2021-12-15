#include "game.h"

/**
 * @brief Procedura che gestisce il gioco
 * 
 * @param win 
 * @param p
 */
void mainGame(WINDOW* win, Point p){
    srand(time(NULL));
    pid_t pidEnemyShips[NUMBER_ENEMY_SHIPS], allyShip, printObject;
    int fileDes[DIM_PIPE], fileDesPrint[DIM_PIPE], fileDesAliens[NUMBER_ENEMY_SHIPS][DIM_PIPE];
    int fileDesObjectsActive[DIM_PIPE];
    int i;
    
    initializePipe(fileDes);
    initializeNonBlockingPipe(fileDesPrint);
    initializeNonBlockingPipe(fileDesStatus);
    for(i = 0; i<NUMBER_ENEMY_SHIPS;i++){
        initializeNonBlockingPipe(fileDesAliens[i]);
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
            for(i = 0; i < NUMBER_ENEMY_SHIPS; i++){ //cicla finchè non genera tutti gli alieni
                switch (pidEnemyShips[i] = fork()) { //creazione processi alieni
                    case PROCESS_RETURN_FAILURE:
                        printExceptions(TYPE_EXCEPTION_PROCESS_CREATION_FAILURE);
                        break;
                    case PROCESS_RETURN_CHILD_PID: //processo figlio che gestisce l'alieno
                        close(fileDesPrint[PIPE_WRITE]);
                        close(fileDesPrint[PIPE_READ]);
                        close(fileDes[PIPE_READ]);
                        enemyShipController(win, p, fileDesAliens[i][PIPE_WRITE], fileDesObjectsActive[PIPE_WRITE], i);
                        break;                   
                }
            }    
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
                        checkCollision(win, p, fileDes[PIPE_READ], fileDesPrint[PIPE_WRITE], fileDesAliens, fileDesObjectsActive[PIPE_READ]);
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
    usleep(200000);
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
    Object ship;
    ship.pos.x = ALLY_BORDER_SPACE + OUTER_STARSHIP;
    ship.pos.y = Y_HSEPARATOR + divideByTwo(p.y - Y_HSEPARATOR) + 1;
    ship.typeObject = ALLY_SHIP_TYPE;
    ship.direction = 0;
    ship.pid = getpid();
    ship.health = 3;
    int nBulletsActive = 0;
    int isBulletShot = false;

    while (true) {
        write(pipeOut, &ship, sizeof(Object));
        moveAllyShip(win, p, &ship.pos.y, &isBulletShot);
        if(isBulletShot && nBulletsActive < MAX_BULLETS_ACTIVE){
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
                            usleep(200000);
                            break;
                    }
            }
        }
    }
}

/**
 * @brief Procedura che gestisce la creazione e lo spostamento della navicella nemica
 * 
 * @param win finestra
 * @param p struttura per risoluzione
 * @param pipeOut pipe di scrittura
 * @param idNumber il famoso M delle specifiche
 */
void enemyShipController(WINDOW* win, Point p, int pipeOut, int pipeInStatus, int idNumber){
    pid_t bomb;
    int i = 0;
    Object alien;
    Status status;
    alien.pos.x = p.x - ALLY_BORDER_SPACE - OUTER_ALIEN;
    alien.pos.y = divideByTwo(Y_HSEPARATOR) + ((p.y-Y_HSEPARATOR)/(NUMBER_ENEMY_SHIPS+1))*(idNumber+1) + OUTER_ALIEN;
    alien.typeObject = ENEMY_SHIP_TYPE;
    alien.direction = 0;
    alien.pid = getpid();
    alien.health=3;
    int timer = 1; // ogni 3 movimenti della nave viene sparata una bomba
    int direction = 0;

    while(true){
        write(pipeOut, &alien, sizeof(Object));
        direction = MIN_RAND + rand()%(MAX_RAND-MIN_RAND+1);
        alien.direction = direction%2 ? UP_DIRECTION : DOWN_DIRECTION;
        Point bombPos;
        bombPos.x = alien.pos.x-1;
        bombPos.y = alien.pos.y;
        timer++;
        if(read(pipeInStatus, &status, sizeof(status)) > 0){
            changeDirection(status, &alien.direction);
            alien.pos.y = alien.direction == UP_DIRECTION ? alien.pos.y-- : alien.pos.y++;
        }
        //TODO modificare timer in base alla difficolta'
        if(timer%15 == 0){
            switch (bomb = fork()) {
                case PROCESS_RETURN_FAILURE:
                    printExceptions(TYPE_EXCEPTION_PROCESS_CREATION_FAILURE);
                    break;
                case PROCESS_RETURN_CHILD_PID:
                    bombController(win,p,bombPos,pipeOut);
            }
        }
        usleep(200000);
        //usleep(4000000);
    }
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
                if(checkPos(p, bullet.pos.y, sizeof(BULLET_SPRITE))){
                    bullet.pos.y--;
                    bullet.pos.x += BULLET_PACE;
                } else {
                    bullet.direction = DOWN_DIRECTION;
                    bullet.pos.y++;
                    bullet.pos.x += BULLET_PACE;
                }
                break;
            case DOWN_DIRECTION:
                if(checkPos(p, bullet.pos.y, sizeof(BULLET_SPRITE))){
                    bullet.pos.y++;
                    bullet.pos.x += BULLET_PACE;
                } else {
                    bullet.direction = UP_DIRECTION;
                    bullet.pos.y--;
                    bullet.pos.x += BULLET_PACE;
                }
            }
        write(pipeOut, &bullet, sizeof(Object));
        usleep(200000);
    }
    kill(getpid(), SIGUSR1);
}

/**
 * @brief Procedura che gestisce la generazione della bomba da parte della navicella nemica
 * 
 * @param pipeOut 
 */
void bombController(WINDOW* win, Point p, Point posAlien, int pipeOut){
    Object bomb;
    bomb.pos.x = posAlien.x-1;
    bomb.pos.y = posAlien.y;
    bomb.typeObject = BOMB_TYPE;
    bomb.direction = 0;

    while(bomb.pos.x > 0){
        bomb.pos.x--;
        usleep(200000);
        write(pipeOut,&bomb,sizeof(Object));
    }
    //usleep(5000000);
    perror("bomba smarties ke muore kadendo");
    kill(getpid(), SIGKILL);
}

/**
 * @brief Procedura che gestisce la visualizzazione degli oggetti nella finestra
 * 
 * @param pipeOut 
 */
void printObjects (WINDOW* win, Point p, int pipeIn) {
    Object obj;

    while (true) {
        if(read(pipeIn, &obj, sizeof(Object)) > 0) {

            switch(obj.typeObject){
                case ALLY_SHIP_TYPE:
                    printStarShip(win, obj);
                    break;
                case ENEMY_SHIP_TYPE:
                    printStarShip(win, obj);
                    break;
                case BULLET_TYPE:
                    printBullet(win, obj);
                    break;
                case BOMB_TYPE:
                    printBullet(win, obj);
                    break;
            }      
        }
        wmove(win, Y_HSEPARATOR,0);
        whline(win, ACS_HLINE,p.x);
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
void checkCollision (WINDOW* win, Point p, int pipeIn, int pipeOutPrint, int pipeInAliens[NUMBER_ENEMY_SHIPS][DIM_PIPE], int pipeOutObjectsActive) {
    Object allyShip, obj;
    pid_t bullets[NUMBER_BULLETS] = {PROCESS_RETURN_CHILD_PID};
    int i, pipeInAlien;
    Status status;
    Object entitiesActive[ENTITY_TYPES][MAX_ENTITY_ACTIVE+1];
    entityArrayInitializer(entitiesActive);
    do {
        /*if(read(pipeIn, &obj, sizeof(Object)) > 0){
            addEntity(entitiesActive, obj);
            switch(obj.typeObject){
                case ALLY_SHIP_TYPE:
                    write(pipeOutPrint, &obj, sizeof(Object));
                    break;
                case BULLET_TYPE:
                    write(pipeOutPrint, &obj, sizeof(Object));
                    break;
            }
        }*/
        
        for(i = 0; i<NUMBER_ENEMY_SHIPS; i++){  //ciclo tutte le pipe
            pipeInAlien = pipeInAliens[i][PIPE_READ];  //per ogni pipe, faccio la read per recuperare l'oggetto passato
            if(read(pipeInAlien, &obj, sizeof(Object)) > 0){   
                addEntity(entitiesActive, obj);
                switch(obj.typeObject){  //switch del type object per la stampa
                    case ENEMY_SHIP_TYPE:
                        if(!checkPos(p, obj.pos.y, ALIEN_SIZE)){
                            status.collision = BOUNCE_COLLISION;
                        } else {
                            status.collision = NO_COLLISION;
                        }
                        
                        if(write(pipeOutStatus, &status, sizeof(status)) > 0){
                            
                        }
                        break;
                    case BOMB_TYPE:
                        if(write(pipeOutPrint, &obj, sizeof(Object)) > 0){
                            
                        };
                    break;
                }
            }
            
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
    switch(ship.typeObject){
        case ALLY_SHIP_TYPE:
            mvwprintw(win, ship.pos.y-OUTER_STARSHIP, ship.pos.x, BLANK_SPACES_STARSHIP);
            mvwprintw(win, ship.pos.y+OUTER_STARSHIP, ship.pos.x, BLANK_SPACES_STARSHIP);

            char allySprite[ROWS_STARSHIP][COLS_STARSHIP] = STARSHIP;

            for(i=0;i<ROWS_STARSHIP;i++){
                for(j=0;j<COLS_STARSHIP;j++){
                    y = ship.pos.y-divideByTwo(STARSHIP_SIZE) + i;
                    mvwaddch(win, y, ship.pos.x+j, allySprite[i][j]);
                }
            }
            break;
        case ENEMY_SHIP_TYPE:
            y = ship.direction == UP_DIRECTION ? OUTER_ALIEN*2 : OUTER_ALIEN*(-2);
            mvwprintw(win, ship.pos.y+y, ship.pos.x-OUTER_ALIEN, BLANK_SPACES_ALIEN);

            char alienSprite[ROWS_ALIEN][COLS_ALIEN] = ENEMYSHIP;
            for(i=0;i<ROWS_ALIEN;i++){
                for(j=0;j<COLS_ALIEN;j++){
                    y = ship.pos.y-divideByTwo(ALIEN_SIZE) + i;
                    mvwaddch(win, y, ship.pos.x+j, alienSprite[i][j]);
                }
            }
            break;
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
            x = bullet.pos.x - BULLET_PACE; 
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
    //timeout(100);
    cbreak();
    halfdelay(true);
    int arrow = wgetch(win);
    if(arrow == ASCII_CODE_SPACE_BAR) {
        *isBulletShot = true;
    }
    keyPadSelector(win, Y_HSEPARATOR+STARSHIP_SIZE, p.y-STARSHIP_SIZE, arrow, yPos);
    nocbreak();
}

bool checkPos (Point p, int yPos, int size) {
    return yPos > Y_HSEPARATOR+(size+1) && yPos < p.y-size;
}