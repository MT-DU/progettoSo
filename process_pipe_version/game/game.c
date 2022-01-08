#include "game.h"

/**
 * @brief Procedura principale da cui parte il gioco
 * 
 * @param win 
 * @param p
 * @param difficultyMode
 */
void mainGame(WINDOW* win, Point p, Difficulty difficultyMode){
    srand(time(NULL));
    pid_t pidEnemyShips[NUMBER_ENEMY_SHIPS_HARD], allyShip;
    int fileDes[DIM_PIPE];
    int i, status;
    EndGame gameStatus;

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
            usleep(500);
            for(i = 0; i < getMaxAlien(difficultyMode); i++){ //cicla finchè non genera tutti gli alieni
                switch (pidEnemyShips[i] = fork()) { //creazione processi alieni
                    case PROCESS_RETURN_FAILURE:
                        printExceptions(TYPE_EXCEPTION_PROCESS_CREATION_FAILURE);
                        break;
                    case PROCESS_RETURN_CHILD_PID: //processo figlio che gestisce l'alieno
                        close(fileDes[PIPE_READ]);
                        enemyShipController(win, p, fileDes[PIPE_WRITE], i, difficultyMode);
                        break;                   
                }
            }    
            close(fileDes[PIPE_WRITE]);
            gameStatus = printObjects(win, p, fileDes[PIPE_READ], difficultyMode);
            endGame(pidEnemyShips, allyShip, difficultyMode);
    }
    endGamePrint(win, p, gameStatus);
    while(wait(0)>0);
}

/**
 * @brief Procedura che visualizza la hud nella window
 * 
 * @param win 
 * @param p 
 * @param allyShipHealth 
 * @param nAliens 
 */
void hudGame(WINDOW* win, Point p, int allyShipHealth, int nAliens){
    
    int i;
    int length = strlen(HEALTH_TEXT_HUD)+PASSO;
    int healthPosX = HEALTH_BAR_POS_X+length;
    
    mvwprintw(win, TEXT_HUD_POS_Y, HEALTH_BAR_POS_X, HEALTH_TEXT_HUD);
    mvwprintw(win, TEXT_HUD_POS_Y, healthPosX, HEALTH_SPACE_HUD);
    pickColor(win, PAIR_COLOR_HEART);
    for(i=0;i<allyShipHealth;i++){
        mvwprintw(win, TEXT_HUD_POS_Y, healthPosX+i*3, "<3 ");
    }
    turnOffColor(win, PAIR_COLOR_HEART);
    pickColor(win, PAIR_COLOR_ALIENS_REMAINING);
    mvwprintw(win, TEXT_HUD_POS_Y, NUMBER_ALIENS_BAR_POS_X, ALIENS_TEXT_HUD, nAliens);
    turnOffColor(win, PAIR_COLOR_ALIENS_REMAINING);
    wmove(win, Y_HSEPARATOR, 0);
    whline(win, ACS_HLINE, p.x);
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
    ship.pos.x = ALLY_BORDER_SPACE;
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
 * @param win
 * @param p
 * @param pipeOut
 * @param idNumber numero progressivo univoco di ciascun alieno
 * @param difficultyMode
 */
void enemyShipController(WINDOW* win, Point p, int pipeOut, int idNumber, Difficulty difficultyMode) {
    pid_t bomb;
    Object alien;
    int statusPid, i = 0, numSpostamenti = 3;
    bool generateBomb = true;
    // posizione iniziale dell'alieno con numero progressivo uguale a idNumber
    i = idNumber / MAX_ALIENS_FOR_ROW;
    alien.pos.x = p.x - (ALLY_BORDER_SPACE + OUTER_ALIEN + SPACE_BETWEEN_ALIENS)*(i+PASSO);
    i = idNumber % MAX_ALIENS_FOR_ROW;
    alien.pos.y = Y_HSEPARATOR + divideByTwo(divideByTwo(p.y - Y_HSEPARATOR))*(i+PASSO);
    // inizializzaizione delle informazioni dell'alieno
    alien.typeObject = ENEMY_SHIP_TYPE;
    alien.direction = DOWN_DIRECTION;
    alien.pid = getpid();
    alien.idObj = idNumber;

    while(true){
        write(pipeOut, &alien, sizeof(Object));
        /* SPOSTAMENTO SINCRONO DEGLI ALIENI */
        switch(alien.direction){
            case UP_DIRECTION:
                if(numSpostamenti == MIN_MOVE_ALIEN){
                    alien.direction = DOWN_DIRECTION;
                    alien.pos.x--;
                }else{
                    alien.pos.y--;
                }
                numSpostamenti--;
                break;
            case DOWN_DIRECTION:
                if(numSpostamenti == MAX_MOVE_ALIEN){
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
                    bombController(win, p, alien.pos, pipeOut, difficultyMode);
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
        usleep(getDelay(difficultyMode));
    }
}

/**
 * @brief Procedura che gestisce la generazione del proiettile da parte della navicella alleata
 * 
 * @param win 
 * @param p 
 * @param posShip 
 * @param direction 
 * @param pipeOut 
 */
void bulletController(WINDOW* win, Point p, Point posShip, Direction direction, int pipeOut){
    Object bullet;
    bullet.pos.x = posShip.x + COLS_STARSHIP;
    bullet.pos.y = posShip.y;
    bullet.typeObject = BULLET_TYPE;
    bullet.direction = direction;
    bullet.idObj = direction; //assegno come id unico la direzione
    bullet.pid = getpid();
    while(p.x >= bullet.pos.x){
        switch (bullet.direction) { 
            case UP_DIRECTION:
                if(checkPos(p, bullet.pos.y, PASSO)){
                    bullet.pos.y--;
                    bullet.pos.x += BULLET_PACE;
                } else {
                    bullet.direction = DOWN_DIRECTION;
                    bullet.pos.y++;
                    bullet.pos.x += BULLET_PACE;
                }
                break;
            case DOWN_DIRECTION:
                if(checkPos(p, bullet.pos.y, PASSO)){
                    bullet.pos.y++;
                    bullet.pos.x += BULLET_PACE;
                } else {
                    bullet.direction = UP_DIRECTION;
                    bullet.pos.y--;
                    bullet.pos.x += BULLET_PACE;
                }
            }
        write(pipeOut, &bullet, sizeof(Object));
        usleep(100000);
    }
    _exit(SIGUSR1);
}

/**
 * @brief Procedura che gestisce la generazione della bomba da parte della navicella nemica
 * 
 * @param win 
 * @param p 
 * @param posAlien 
 * @param pipeOut 
 * @param difficultyMode 
 */
void bombController(WINDOW* win, Point p, Point posAlien, int pipeOut, Difficulty difficultyMode){
    Object bomb;
    bomb.pos.x = posAlien.x-PASSO;
    bomb.pos.y = posAlien.y;
    bomb.typeObject = BOMB_TYPE;
    bomb.direction = UP_DIRECTION;
    bomb.pid = getpid();
    while(bomb.pos.x > 0){
        bomb.pos.x--;
        usleep(50000);
        write(pipeOut, &bomb, sizeof(Object));
    }
    _exit(SIGUSR2);
}

/**
 * @brief Procedura che gestisce la visualizzazione degli oggetti nella finestra e le loro collisioni
 * 
 * @param win 
 * @param p 
 * @param pipeIn 
 * @param difficultyMode 
 */
EndGame printObjects (WINDOW* win, Point p, int pipeIn, Difficulty difficultyMode) {
    Object allyShip, aliens[NUMBER_ENEMY_SHIPS_HARD], obj;
    Object bullets[MAX_BULLETS_ACTIVE], bomb[NUMBER_ENEMY_SHIPS_HARD];
    objectArrayInitializer(bullets, MAX_BULLETS_ACTIVE);
    objectArrayInitializer(bomb, NUMBER_ENEMY_SHIPS_HARD);
    objectArrayInitializer(aliens, NUMBER_ENEMY_SHIPS_HARD);
    int status, i, allyShipHealth = getMaxHealth(difficultyMode), aliensHealth[NUMBER_ENEMY_SHIPS_HARD], nAliensAlive = getMaxAlien(difficultyMode);
    bool takeHealth = false, alienAllyCollision = false, firstKill = false;
    EndGame gameStatus = CONTINUE;
    initializeArray(aliensHealth, NUMBER_ENEMY_SHIPS_HARD, MAX_HEALTH_ALIEN);
    wclear(win);
    do {
        usleep(500);
        if(read(pipeIn, &obj, sizeof(Object)) > 0) { // lettura dalla pipe della navicella alleata
            switch(obj.typeObject){
                case ALLY_SHIP_TYPE:
                    allyShip = obj;
                    allyShip.health = allyShipHealth;
                    hudGame(win, p, allyShip.health, nAliensAlive);
                    
                    printStarShip(win, p, allyShip);
                    break;
                case BULLET_TYPE:
                    bullets[obj.idObj] = obj;
                    for(i=0; i<getMaxAlien(difficultyMode); i++){
                        if(aliens[i].pid != UNDEFINED_PID && checkAlienBulletCollision(aliens[i].pos, bullets[obj.idObj].pos)){
                            aliensHealth[i]--;
                            aliens[i].health = aliensHealth[i];
                            kill(bullets[obj.idObj].pid, SIGUSR1);
                            wclear(win);
                            hudGame(win, p, allyShip.health, nAliensAlive);
                            bullets[obj.idObj].pid = UNDEFINED_PID;
                            clearObjects(win, p, bullets[obj.idObj]);
                            if(aliens[i].health == NO_HEALTH_REMAINING){
                                kill(aliens[i].pid, SIGUSR1);
                                aliens[i].pid = UNDEFINED_PID;
                                clearObjects(win, p, aliens[i]);
                                firstKill = true;
                            }
                        }
                    }
                    break;
                case ENEMY_SHIP_TYPE:
                    aliens[obj.idObj] = obj;
                    aliens[obj.idObj].health = aliensHealth[obj.idObj];
                    
                    if(checkAllyAlienCollision(aliens[obj.idObj].pos)){
                        alienAllyCollision=true;
                    }
                    break;
                case BOMB_TYPE:
                    bomb[obj.idObj] = obj;
                    if(checkAllyBombCollision(allyShip.pos, bomb[obj.idObj].pos)){
                        allyShipHealth--;
                        kill(bomb[obj.idObj].pid, SIGUSR2);
                        clearObjects(win, p, obj);
                    }
                    break;
            }      
        }        

        for(i = 0; i<NUMBER_BULLETS; i++){ 
            if(bullets[i].pid != UNDEFINED_PID){
                printBullet(win, bullets[i]);
            }
        }

        //stampa degli oggetti nemici
        
        for(i = 0; i<getMaxAlien(difficultyMode); i++){  
            if(aliens[i].pid != UNDEFINED_PID){
                printStarShip(win, p, aliens[i]);
            }
            if(bomb[i].pid != UNDEFINED_PID){
                printBullet(win, bomb[i]);
            }
        }

        for(i=Y_HSEPARATOR;i<p.y;i++){
            mvwaddch(win, i, 0, BLANK_SPACE);
        }
        
        if(firstKill){
            nAliensAlive = countObjects(aliens, getMaxAlien(difficultyMode));
        }
        
        
        gameStatus = isGameOver(allyShip.health, alienAllyCollision, nAliensAlive);
        wrefresh(win);
    } while (gameStatus == CONTINUE);
    return gameStatus;
}

/**
 * @brief Funzione che determina la condizione di vittoria, sconfitta o di continuo del gioco
 * 
 * @param healthAllyShip 
 * @param alienAllyCollision 
 * @param nAliensAlive 
 * @return EndGame 
 */
EndGame isGameOver (int healthAllyShip, bool alienAllyCollision, int nAliensAlive){
    if(healthAllyShip == 0 || alienAllyCollision){
        return DEFEAT;
    }
    
    return nAliensAlive == 0 ? VICTORY : CONTINUE;
}

/**
 * @brief Procedura che ha la funzione di stampare la navicella alleata e gli alieni di primo
 * e secondo livello
 * 
 * @param win 
 * @param p 
 * @param ship 
 */
void printStarShip (WINDOW* win, Point p, Object ship) {
    int i,j, y;
    switch(ship.typeObject){
        /* STAMPA NAVICELLA ALLEATA */
        case ALLY_SHIP_TYPE:
            pickColor(win, PAIR_COLOR_ALLY_SHIP);
            char allySprite[ROWS_STARSHIP][COLS_STARSHIP] = STARSHIP;
            mvwprintw(win, ship.pos.y-OUTER_STARSHIP, ship.pos.x, BLANK_SPACES_STARSHIP);
            mvwprintw(win, ship.pos.y+OUTER_STARSHIP, ship.pos.x, BLANK_SPACES_STARSHIP);
            for(i=0;i<ROWS_STARSHIP;i++){
                for(j=0;j<COLS_STARSHIP;j++){
                    y = ship.pos.y-divideByTwo(STARSHIP_SIZE) + i;
                    mvwaddch(win, y, ship.pos.x+j, allySprite[i][j]);
                }
            }
            turnOffColor(win, PAIR_COLOR_ALLY_SHIP);
            break;
            /* STAMPA ALIENI */
        case ENEMY_SHIP_TYPE:
            y = ship.direction == UP_DIRECTION ? OUTER_ALIEN*DISTANCE_FROM_CENTER_ALIEN : OUTER_ALIEN*(-DISTANCE_FROM_CENTER_ALIEN);
            mvwprintw(win, ship.pos.y+y, ship.pos.x-OUTER_ALIEN, BLANK_SPACES_ALIEN);
            /* CANCELLA POSIZIONE PRECEDENTE DELL'ALIENO */
            for(i=0;i<ALIEN_SIZE+2;i++){
                mvwaddch(win, ship.pos.y+i-2, ship.pos.x+DISTANCE_FROM_CENTER_ALIEN, BLANK_SPACE);
                mvwaddch(win, ship.pos.y+i-2, ship.pos.x+DISTANCE_FROM_CENTER_ALIEN+PASSO, BLANK_SPACE);
                mvwaddch(win, ship.pos.y-DISTANCE_FROM_CENTER_ALIEN, ship.pos.x+i-2, BLANK_SPACE);
                mvwaddch(win, ship.pos.y+DISTANCE_FROM_CENTER_ALIEN, ship.pos.x+i-2, BLANK_SPACE);
            }
            
            if(ship.health == MAX_HEALTH_ALIEN){
                pickColor(win, PAIR_COLOR_ALIEN);
                char alienSprite[ROWS_ALIEN][COLS_ALIEN] = ENEMYSHIP;
                for(i=0;i<ROWS_ALIEN;i++){
                    for(j=0;j<COLS_ALIEN;j++){
                        y = ship.pos.y-divideByTwo(ALIEN_SIZE) + i;
                        mvwaddch(win, y, ship.pos.x+j, alienSprite[i][j]);
                    }
                }
                turnOffColor(win, PAIR_COLOR_ALIEN);
            }else{
                if(ship.health == MAX_HEALTH_ALIEN -1){
                    pickColor(win, PAIR_COLOR_ALIEN_HALF_HEALTH);
                }else{
                    pickColor(win, PAIR_COLOR_ALIEN_LOW_HEALTH);
                }
                char alienSprite[ROWS_ALIEN][COLS_ALIEN] = ENEMYSHIP_LEVEL_TWO;
                for(i=0;i<ROWS_ALIEN;i++){
                    for(j=0;j<COLS_ALIEN;j++){
                        y = ship.pos.y-divideByTwo(ALIEN_SIZE) + i;
                        mvwaddch(win, y, ship.pos.x+j, alienSprite[i][j]);
                    }
                }
                turnOffColor(win, PAIR_COLOR_ALIEN_HALF_HEALTH);
                turnOffColor(win, PAIR_COLOR_ALIEN_LOW_HEALTH);
            }
            break;
    }
}

/**
 * @brief Procedura che stampa il proiettile o la bomba in base all'oggetto passato
 * 
 * @param win 
 * @param ship 
 */
void printBullet (WINDOW* win, Object bullet) {
    int y, x;
    switch (bullet.typeObject) {
        case BULLET_TYPE:
            pickColor(win, PAIR_COLOR_BULLET);
            y = bullet.direction == UP_DIRECTION ? bullet.pos.y + PASSO : bullet.pos.y - PASSO;
            x = bullet.pos.x - BULLET_PACE; 
            mvwaddch(win, y, x, BLANK_SPACE);
            mvwaddch(win, bullet.pos.y, bullet.pos.x, BULLET_SPRITE);
            turnOffColor(win, PAIR_COLOR_BULLET);
            break;
        case BOMB_TYPE:
            pickColor(win, PAIR_COLOR_BOMB);
            y = bullet.pos.y;
            x = bullet.pos.x + PASSO;
            mvwaddch(win, y, x, BLANK_SPACE);
            mvwaddch(win, bullet.pos.y, bullet.pos.x, BOMB_SPRITE);
            turnOffColor(win, PAIR_COLOR_BOMB);
            break;
    }
}

/**
 * @brief Procedura che gestisce i comandi della navicella
 * 
 * @param win 
 * @param p 
 * @param yPos 
 * @param isBulletShot 
 */
void moveAllyShip (WINDOW* win, Point p, int* yPos, int* isBulletShot) {
    keypad(win, TRUE);
    cbreak();
    wtimeout(win, 1);
    int arrow = wgetch(win);
    if(arrow == ASCII_CODE_SPACE_BAR) {
        *isBulletShot = true;
    }
    keyPadSelector(win, Y_HSEPARATOR+STARSHIP_SIZE, p.y-STARSHIP_SIZE, arrow, yPos);
    nocbreak();
}

/**
 * @brief Funzione che verifica se la posizione dell'oggetto passato e' nei limiti definiti dallo schermo
 * 
 * @param p 
 * @param yPos 
 * @param size 
 * @return true 
 * @return false 
 */
bool checkPos (Point p, int yPos, int size) {
    return yPos > Y_HSEPARATOR+size && yPos < p.y-size;
}

/**
 * @brief Procedura che killa la navicella alleata e gli alieni
 * 
 * @param aliens 
 * @param allyShip 
 * @param difficultyMode 
 */
void endGame(pid_t aliens[], pid_t allyShip, Difficulty difficultyMode){
    int i;
    for(i=0; i<getMaxAlien(difficultyMode); i++){
        kill(aliens[i], SIGUSR2);
    }
    kill(allyShip, SIGUSR2);
}

/**
 * @brief Funzione che restituisce la dimensione da utilizzare in base alla difficolta'
 * 
 * @param difficultyMode 
 * @return int 
 */
int getMaxAlien(Difficulty difficultyMode){
    return difficultyMode == EASY ? NUMBER_ENEMY_SHIPS_EASY : NUMBER_ENEMY_SHIPS_HARD;
}

/**
 * @brief Funzione che restituisce la vita massima da utilizzare in base alla difficolta'
 * 
 * @param difficultyMode 
 * @return int 
 */
int getMaxHealth(Difficulty difficultyMode){
    return difficultyMode == EASY ? MAX_HEALTH_ALLY_EASY : MAX_HEALTH_ALLY_HARD;
}

/**
 * @brief Get the Delay object
 * 
 * @param difficultyMode 
 * @return int 
 */
int getDelay(Difficulty difficultyMode){
    return difficultyMode == EASY ? DELAY_ALIEN_EASY : DELAY_ALIEN_HARD;
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