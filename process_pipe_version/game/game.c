#include "game.h"

/**
 * @brief Procedura che gestisce il gioco
 * 
 * @param win 
 * @param p
 */
void mainGame(WINDOW* win, Point p){
    srand(time(NULL));
    pid_t pidEnemyShips[NUMBER_ENEMY_SHIPS], allyShip;
    int fileDes[DIM_PIPE];
    int i, status;
    
    initializePipe(fileDes);
    switch (allyShip = fork()) { //creazione processo navicella alleata
        case PROCESS_RETURN_FAILURE:
            printExceptions(TYPE_EXCEPTION_PROCESS_CREATION_FAILURE);
            break;
        case PROCESS_RETURN_CHILD_PID: //processo figlio che gestisce la navicella alleata
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
                        close(fileDes[PIPE_READ]);
                        enemyShipController(win, p, fileDes[PIPE_WRITE], i);
                        break;                   
                }
            }    
            close(fileDes[PIPE_WRITE]);
            printObjects(win, p, fileDes[PIPE_READ]);
            endGame(pidEnemyShips, allyShip);
    }
    while(wait(0)>0);
}

/**
 * @brief Procedura che visualizza la hud nella window
 * 
 * @param win 
 * @param p 
 */
void hudGame(WINDOW* win, Point p, int allyShipHealth, int timer, int nAliens){
    
    int i;
    int length = strlen(HEALTH_TEXT_HUD)+1;
    int healthPosX = HEALTH_BAR_POS_X+length;
    
    mvwprintw(win, TEXT_HUD_POS_Y, HEALTH_BAR_POS_X, HEALTH_TEXT_HUD);
    mvwprintw(win, TEXT_HUD_POS_Y, healthPosX, HEALTH_SPACE_HUD);

    for(i=0;i<allyShipHealth;i++){
        mvwprintw(win, TEXT_HUD_POS_Y, healthPosX+i*3, "<3 ");
    }
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
    ship.pos.x = ALLY_BORDER_SPACE + STARSHIP_SIZE;
    ship.pos.y = Y_HSEPARATOR + divideByTwo(p.y - Y_HSEPARATOR);
    ship.typeObject = ALLY_SHIP_TYPE;
    ship.direction = 0;
    ship.pid = getpid();
    int isBulletShot = false, canShoot = true;
    int status1 = 0, status2 = 0;

    while (true) {
        write(pipeOut, &ship, sizeof(Object));
        moveAllyShip(win, p, &ship.pos.y, &isBulletShot);
        if(isBulletShot && canShoot){
            switch(bullets[UP_DIRECTION] = fork()){
                case PROCESS_RETURN_FAILURE:
                    printExceptions(TYPE_EXCEPTION_PROCESS_CREATION_FAILURE);
                    break;
                case PROCESS_RETURN_CHILD_PID:
                    bulletController(win, p, ship.pos, UP_DIRECTION, pipeOut);
                    break;
            }
            switch(bullets[DOWN_DIRECTION] = fork()){
                case PROCESS_RETURN_FAILURE:
                    printExceptions(TYPE_EXCEPTION_PROCESS_CREATION_FAILURE);
                    break;
                case PROCESS_RETURN_CHILD_PID:
                    bulletController(win, p, ship.pos, DOWN_DIRECTION, pipeOut);
                    break;
            }
            canShoot = false;
            status1 = OBJ_ALIVE;
            status2 = OBJ_ALIVE;
        }
        // catturo il figlio zombie e aggiorno lo status
        waitpid(bullets[UP_DIRECTION], &status1, WNOHANG); 
        waitpid(bullets[DOWN_DIRECTION], &status2, WNOHANG);
        // se lo stato è diverso da OBJ_ALIVE, significa che il figlio è uscito
        if(status1 != OBJ_ALIVE && status2 != OBJ_ALIVE){ 
            canShoot = true; 
            isBulletShot = false;
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
void enemyShipController(WINDOW* win, Point p, int pipeOut, int idNumber){
    pid_t bomb;
    Object alien;
    int statusPid, i = 0, numSpostamenti = 3, timer = 1;
    bool generateBomb = true;
    // posizione iniziale dell'alieno con numero progressivo uguale a idNumber
    i = idNumber / MAX_ALIENS_FOR_ROW;
    alien.pos.x = p.x - (ALLY_BORDER_SPACE + OUTER_ALIEN + SPACE_BETWEEN_ALIENS)*(i+1);
    i = idNumber % MAX_ALIENS_FOR_ROW;
    alien.pos.y = Y_HSEPARATOR + divideByTwo(divideByTwo(p.y - Y_HSEPARATOR))*(i+1);
    // inizializzaizione delle informazioni dell'alieno
    alien.typeObject = ENEMY_SHIP_TYPE;
    alien.direction = DOWN_DIRECTION;
    alien.pid = getpid();
    alien.idObj = idNumber;

    while(true){
        write(pipeOut, &alien, sizeof(Object));
        switch(alien.direction){
            case UP_DIRECTION:
                if(numSpostamenti == 1){
                    alien.direction = DOWN_DIRECTION;
                    alien.pos.x--;
                }else{
                    alien.pos.y--;
                }
                numSpostamenti--;
                break;
            case DOWN_DIRECTION:
                if(numSpostamenti == 5){
                    alien.direction = UP_DIRECTION;
                    alien.pos.x--;
                }else{
                    alien.pos.y++;
                }
                numSpostamenti++;                
                break; 
        }
        //alien.pos.y = alien.direction == UP_DIRECTION ? alien.pos.y-- : alien.pos.y++;
        if(generateBomb && rand()%10 == 0){
            switch (bomb = fork()) {
                case PROCESS_RETURN_FAILURE:
                    printExceptions(TYPE_EXCEPTION_PROCESS_CREATION_FAILURE);
                    break;
                case PROCESS_RETURN_CHILD_PID:
                    bombController(win, p, alien.pos, pipeOut);
            }
            generateBomb = false;
            statusPid = OBJ_ALIVE;
        }
        // catturo il figlio zombie e aggiorno lo status
        waitpid(bomb, &statusPid, WNOHANG); 
        // se lo stato è diverso da OBJ_ALIVE, significa che il figlio è uscito
        if(statusPid != OBJ_ALIVE){ 
            generateBomb = true; 
        }   
        usleep(200000);
    }
}

/**
 * @brief Procedura che gestisce la generazione del proiettile da parte della navicella alleata
 * 
 * @param pipeOut 
 */
void bulletController(WINDOW* win, Point p, Point posShip, Direction direction, int pipeOut){
    Object bullet;
    bullet.pos.x = posShip.x + COLS_STARSHIP;
    bullet.pos.y = posShip.y;
    bullet.typeObject = BULLET_TYPE;
    bullet.direction = direction;
    bullet.idObj = direction; //assegno come id unico la direzione
    while(p.x >= bullet.pos.x){
        switch (bullet.direction) { 
            case UP_DIRECTION:
                if(checkPos(p, bullet.pos.y, 1)){
                    bullet.pos.y--;
                    bullet.pos.x += BULLET_PACE;
                } else {
                    bullet.direction = DOWN_DIRECTION;
                    bullet.pos.y++;
                    bullet.pos.x += BULLET_PACE;
                }
                break;
            case DOWN_DIRECTION:
                if(checkPos(p, bullet.pos.y, 1)){
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
    _exit(SIGUSR1);
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
    bomb.direction = UP_DIRECTION;
    bomb.pid = getpid();
    while(bomb.pos.x > 0){
        bomb.pos.x--;
        usleep(200000);
        write(pipeOut, &bomb, sizeof(Object));
    }
    _exit(SIGUSR2);
}

/**
 * @brief Procedura che gestisce la visualizzazione degli oggetti nella finestra
 * 
 * @param pipeOut 
 */
void printObjects (WINDOW* win, Point p, int pipeIn) {
    Object allyShip, aliens[NUMBER_ENEMY_SHIPS], obj;
    Object bullets[MAX_BULLETS_ACTIVE], bomb[NUMBER_ENEMY_SHIPS];
    objectArrayInitializer(bullets, MAX_BULLETS_ACTIVE);
    objectArrayInitializer(bomb, NUMBER_ENEMY_SHIPS);
    objectArrayInitializer(aliens, NUMBER_ENEMY_SHIPS);
    int status, i, allyShipHealth = MAX_HEALTH_ALIEN, aliensHealth[NUMBER_ENEMY_SHIPS] = {MAX_HEALTH_ALIEN};
    bool takeHealth = false, alienAllyCollision = false;
    while (!isGameOver(allyShip.health, alienAllyCollision)) {
        if(read(pipeIn, &obj, sizeof(Object)) > 0) { // lettura dalla pipe della navicella alleata
            switch(obj.typeObject){
                case ALLY_SHIP_TYPE:
                    allyShip = obj;
                    allyShip.health = allyShipHealth;
                    //mvwprintw(win, obj.pos.y+3, obj.pos.x, "O"); TODO
                    //mvwprintw(win, obj.pos.y, obj.pos.x+7, "I");
                    printStarShip(win, p, allyShip);
                    break;
                case BULLET_TYPE:
                    bullets[obj.idObj] = obj;
                    for(i=0; i<NUMBER_ENEMY_SHIPS; i++){
                        if(checkAlienBulletCollision(aliens[i].pos, bullets[obj.idObj].pos)){
                            aliensHealth[i]--;
                            aliens[i].health = aliensHealth[i];
                            kill(bullets[obj.idObj].pid, SIGUSR1);
                        }
                    }
                    printBullet(win, bullets[obj.idObj]);

                    break;
                case ENEMY_SHIP_TYPE:
                    aliens[obj.idObj] = obj;
                    aliens[obj.idObj].health = aliensHealth[obj.idObj];

                    if(checkAllyAlienCollision(allyShip.pos, aliens[obj.idObj].pos)){
                        alienAllyCollision=true;
                    }
                    break;
                case BOMB_TYPE:
                    bomb[obj.idObj] = obj;
                    if(checkAllyBombCollision(allyShip.pos, bomb[obj.idObj].pos)){
                        allyShipHealth--;
                        kill(bomb[obj.idObj].pid, SIGUSR2);
                    }
                    break;
            }      
        }        

        //stampa degli oggetti nemici
        for(i = 0; i<NUMBER_ENEMY_SHIPS; i++){  //ciclo tutte le pipe degli alieni
            printStarShip(win, p, aliens[i]);
            printBullet(win, bomb[i]);
        }

        for(i=0;i<p.y;i++){
            mvwaddch(win,i,0,BLANK_SPACE);
        }
        
        //STAMPA RIGA ORIZZONTALE
        wmove(win, Y_HSEPARATOR,0);
        whline(win, ACS_HLINE,p.x);
        hudGame(win, p, allyShip.health, 100, countObjects(aliens, NUMBER_ENEMY_SHIPS));
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

/*
Status checkCollision (WINDOW* win, Point p, Object* obj, Object array[], int size){
    int i;
    bool collisionCheck = false;
    Status status;
    status.collision = NO_COLLISION;
    TypeObject arrayType = array[0].typeObject;
    
    switch(obj->typeObject){
        case ALLY_SHIP_TYPE:
            //mvwprintw(win, 0, 0, "%d", arrayType);
            if(arrayType != UNDEFINED){
                switch(arrayType){
                case BOMB_TYPE:

                    for(i = 0; i<size; i++){
                        //mvwprintw(win, i, i*5, "(%d,%d)", array[i].pos.y, array[i].pos.x);
                        if(checkAllyBombCollision(obj->pos, array[i].pos)){
                            status.collision = obj->health > 1 ? LOSE_HEART_COLLISION : DEATH_COLLISION;
                            kill(array[i].pid, SIGUSR2);
                        }
                        wrefresh(win);
                    }
                    break;
                case ENEMY_SHIP_TYPE:
                    for(i = 0; i<size; i++){
                        if(checkAllyAlienCollision(obj->pos, array->pos)){
                            // perdo la partita
                        }
                    }
                    break;
                //default:
                //    perror("object colliding with ship is neither a bomb nor an alien");
                }
            }
            
            break;
        case ENEMY_SHIP_TYPE:
            switch(arrayType){
                case BULLET_TYPE:
                    
                    break;
                default:
                    //perror("object colliding with alien is neither a bullet nor an alien");
                    break;
            }
            break;
        case BULLET_TYPE:
            switch (arrayType) {
                case BOMB_TYPE:
                    for(i=0;i<size;i++) {
                        if(checkBulletBombCollision(obj->pos,array[i].pos)){
                            // uccidi entrambi i processi bullet e bomb
                        }
                    }
                    break;
                case ENEMY_SHIP_TYPE:
                        for(i=0;i<size;i++){
                            if(checkAlienBulletCollision(obj->pos, array[i].pos)){
                                array[i].health--;
                                status.collision = obj->health > 1 ? LOSE_HEART_COLLISION : DEATH_COLLISION;
                                kill(obj->pid, SIGUSR1);
                            }
                        }
                    break;
            }
    return status;
    }
}
*/
/**
 * @brief funzione che restituisce true se il gioco e' finito o meno 
 * 
 */
bool isGameOver (int healthAllyShip, int alienAllyCollision){
    return healthAllyShip == 0 || alienAllyCollision;
}

// sample of starship
void printStarShip (WINDOW* win, Point p, Object ship) {
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
            
            y = ship.direction == UP_DIRECTION ? OUTER_ALIEN*DISTANCE_FROM_CENTER_ALIEN : OUTER_ALIEN*(-DISTANCE_FROM_CENTER_ALIEN);
            mvwprintw(win, ship.pos.y+y, ship.pos.x-OUTER_ALIEN, BLANK_SPACES_ALIEN);
            
            for(i=0;i<ALIEN_SIZE+2;i++){
                mvwaddch(win, ship.pos.y+i-2, ship.pos.x+DISTANCE_FROM_CENTER_ALIEN, BLANK_SPACE);
                mvwaddch(win, ship.pos.y+i-2, ship.pos.x+DISTANCE_FROM_CENTER_ALIEN+1, BLANK_SPACE);
                mvwaddch(win, ship.pos.y-DISTANCE_FROM_CENTER_ALIEN, ship.pos.x+i-2, BLANK_SPACE);
                mvwaddch(win, ship.pos.y+DISTANCE_FROM_CENTER_ALIEN, ship.pos.x+i-2, BLANK_SPACE);
            }
            
            if(ship.health >= MAX_HEALTH_ALIEN){
                char alienSprite[ROWS_ALIEN][COLS_ALIEN] = ENEMYSHIP;
                for(i=0;i<ROWS_ALIEN;i++){
                    for(j=0;j<COLS_ALIEN;j++){
                        y = ship.pos.y-divideByTwo(ALIEN_SIZE) + i;
                        mvwaddch(win, y, ship.pos.x+j, alienSprite[i][j]);
                    }
                }
            }else{
                char alienSprite[ROWS_ALIEN][COLS_ALIEN] = ENEMYSHIP_LEVEL_TWO;
                for(i=0;i<ROWS_ALIEN;i++){
                    for(j=0;j<COLS_ALIEN;j++){
                        y = ship.pos.y-divideByTwo(ALIEN_SIZE) + i;
                        mvwaddch(win, y, ship.pos.x+j, alienSprite[i][j]);
                    }
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
    return yPos > Y_HSEPARATOR+size && yPos < p.y-size;
}

void endGame(pid_t aliens[], pid_t allyShip){
    int i;
    for(i=0; i<NUMBER_ENEMY_SHIPS; i++){
        kill(aliens[i], SIGUSR2);
    }
    kill(allyShip, SIGUSR2);
}