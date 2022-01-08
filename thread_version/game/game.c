#include "game.h"

Object* allyShipBuffer, *bullets, *aliens, *bombs;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_t tAllyShip, *tNAliens, *tNBombs, *tNBullets;
int firstAlienKilled, isBulletShot, isBombShot, canShoot, *statusBullets, *statusAliens, *statusBombs, numAliens;
WINDOW* winT;
Point p;
EndGame gameStatus = CONTINUE;
Difficulty difficulty;

/**
 * @brief Procedura principale da cui parte il gioco
 * 
 * @param win 
 * @param difficultyMode
 */
void mainGame(WINDOW* win, Difficulty difficultyMode){
    srand(time(NULL));
    int i, status;
    winT = win;
    getmaxyx(winT, p.y, p.x);
    
    /**inizializzazione variabili **/
    void* id = malloc(sizeof(void*));
    gameStatus = CONTINUE;
    difficulty = difficultyMode;
    numAliens = getMaxAlien(difficultyMode);
    int* arg = calloc(numAliens, sizeof(int*));
    initializeBuffers();
    statusBullets[UP_DIRECTION] = DEFAULT_VALUE;
    statusBullets[DOWN_DIRECTION] = DEFAULT_VALUE;
    firstAlienKilled = 0, isBulletShot = 0, isBombShot = 0, canShoot = 1;
    /** creazione dei thread **/
    pthread_create(&tAllyShip, NULL, allyShipController, NULL);
    pthread_detach(tAllyShip);
    for(i=0;i<numAliens;i++){
        pthread_mutex_lock(&mutex);
        arg[i] = i;
        pthread_mutex_unlock(&mutex);
        pthread_create(&tNAliens[i], NULL, enemyShipController, (void*) &arg[i]);
        pthread_detach(tNAliens[i]);
    }
    /* Chiamata alla funzione che gestisce la stampa */
    gameStatus = printObjects();
    
    if(pthread_mutex_trylock(&mutex) == 0){
        pthread_mutex_unlock(&mutex);
    }

    pthread_cancel(tAllyShip);
    
    for(i=0;i<numAliens;i++){
        if(statusAliens[i] == OBJ_ALIVE){
            killThread(tNAliens, statusAliens, i);
        }

        if(statusBombs[i] == OBJ_ALIVE){
            killThread(tNBombs, statusBombs, i);
        }        
    }
    for(i=0;i<NUMBER_BULLETS;i++){
        if(statusBullets[i] == OBJ_ALIVE){
            killThread(tNBullets, statusBullets, i);
        }
    }
    
    pthread_mutex_destroy(&mutex);
    freeAll();

    endGamePrint(winT, p, gameStatus);
    usleep(1000);
}

/**
 * @brief Procedura che visualizza la hud nella window
 * 
 * @param allyShipHealth 
 * @param nAliens 
 */
void hudGame(int allyShipHealth, int nAliens){
    
    int i;
    int length = strlen(HEALTH_TEXT_HUD)+PASSO;
    int healthPosX = HEALTH_BAR_POS_X+length;
    
    mvwprintw(winT, TEXT_HUD_POS_Y, HEALTH_BAR_POS_X, HEALTH_TEXT_HUD);
    mvwprintw(winT, TEXT_HUD_POS_Y, healthPosX, HEALTH_SPACE_HUD);
    pickColor(winT, PAIR_COLOR_HEART);
    for(i=0;i<allyShipHealth;i++){
        mvwprintw(winT, TEXT_HUD_POS_Y, healthPosX+i*3, "<3 ");
    }
    turnOffColor(winT, PAIR_COLOR_HEART);
    pickColor(winT, PAIR_COLOR_ALIENS_REMAINING);
    mvwprintw(winT, TEXT_HUD_POS_Y, NUMBER_ALIENS_BAR_POS_X, ALIENS_TEXT_HUD, nAliens);
    turnOffColor(winT, PAIR_COLOR_ALIENS_REMAINING);
}

/**
 * @brief Procedura che gestisce la creazione e l'utilizzo della navicella alleata
 * 
 */
void* allyShipController(){
    int i = 0;
    /* protezione della regione critica per la navicella alleata*/
    pthread_mutex_lock(&mutex);
    allyShipBuffer->pos.x = ALLY_BORDER_SPACE;
    allyShipBuffer->pos.y = Y_HSEPARATOR + divideByTwo(p.y - Y_HSEPARATOR);
    allyShipBuffer->typeObject = ALLY_SHIP_TYPE;
    allyShipBuffer->direction = 0;
    allyShipBuffer->tid = pthread_self();
    allyShipBuffer->health = getMaxHealth(difficulty);
    pthread_mutex_unlock(&mutex);
    Direction direction;
    while (true) {
        usleep(15000);
        moveAllyShip(winT);
        
        if(isBulletShot && canShoot){
            canShoot = false;
            //mvwprintw(winT, 3, 3, "bulletShot: %d, canShoot: %d", isBulletShot, canShoot);

            for(i=0;i<NUMBER_BULLETS;i++){
                direction = i; 
                pthread_create(&tNBullets[i], NULL, bulletController, (void*) direction);
                pthread_detach(tNBullets[i]);
            } 
        }
        /* Protezione per la modifica delle variabili globali */
        pthread_mutex_lock(&mutex);
        if(statusBullets[UP_DIRECTION] == OBJ_DEAD && statusBullets[DOWN_DIRECTION] == OBJ_DEAD){ 
            canShoot = true; 
            isBulletShot = false;
            statusBullets[UP_DIRECTION] = DEFAULT_VALUE;
            statusBullets[DOWN_DIRECTION] = DEFAULT_VALUE;
            initializeObject(bullets[UP_DIRECTION]);
            initializeObject(bullets[DOWN_DIRECTION]);
        }     
        pthread_mutex_unlock(&mutex);
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
void* bulletController(void* directionV){
    Direction dir = (Direction) directionV;
    /* protezione della regione critica per il proiettile */
    pthread_mutex_lock(&mutex);
    bullets[dir].pos.x = allyShipBuffer->pos.x + COLS_STARSHIP;
    bullets[dir].pos.y = allyShipBuffer->pos.y;
    bullets[dir].typeObject = BULLET_TYPE;
    bullets[dir].direction = ((Direction)directionV);
    bullets[dir].idObj = dir; //assegno come id unico la direzione
    bullets[dir].tid = pthread_self();
    pthread_mutex_unlock(&mutex);
    statusBullets[dir] = OBJ_ALIVE;
    void* statusThread = malloc(sizeof(void*));
    while(p.x >= bullets[dir].pos.x){
        usleep(100000);
        switch (bullets[dir].direction) { 
            case UP_DIRECTION:
                if(checkPos(p, bullets[dir].pos.y, PASSO)){
                    pthread_mutex_lock(&mutex);
                    bullets[dir].pos.y--;
                    bullets[dir].pos.x += BULLET_PACE;
                    pthread_mutex_unlock(&mutex);
                } else {
                    pthread_mutex_lock(&mutex);
                    bullets[dir].direction = DOWN_DIRECTION;
                    bullets[dir].pos.y++;
                    bullets[dir].pos.x += BULLET_PACE;
                    pthread_mutex_unlock(&mutex);
                }
                break;
            case DOWN_DIRECTION:
                if(checkPos(p, bullets[dir].pos.y, PASSO)){
                    pthread_mutex_lock(&mutex);
                    bullets[dir].pos.y++;
                    bullets[dir].pos.x += BULLET_PACE;
                    pthread_mutex_unlock(&mutex);

                } else {
                    pthread_mutex_lock(&mutex);
                    bullets[dir].direction = UP_DIRECTION;
                    bullets[dir].pos.y--;
                    bullets[dir].pos.x += BULLET_PACE;
                    pthread_mutex_unlock(&mutex);
                }
            }
    }
    statusBullets[dir] = OBJ_DEAD;
    pthread_exit(NULL);
}
/**
 * @brief Procedura che gestisce la creazione e lo spostamento della navicella nemica
 * 
 * @param idNumberT
 */
void* enemyShipController (void* idNumberT) {
    int arg = *((int*) idNumberT);
    int statustid, i = 0, numSpostamenti = 3;
    bool generateBomb = true;
    /* protezione della regione critica per n-alieno */
    pthread_mutex_lock(&mutex);
    int idNumber = *((int*) idNumberT);
    // posizione iniziale dell'alieno con numero progressivo uguale a idNumber
    i = idNumber / MAX_ALIENS_FOR_ROW;
    aliens[idNumber].pos.x = p.x - (ALLY_BORDER_SPACE + OUTER_ALIEN + SPACE_BETWEEN_ALIENS)*(i+PASSO);
    i = idNumber % MAX_ALIENS_FOR_ROW;
    aliens[idNumber].pos.y = Y_HSEPARATOR + divideByTwo(divideByTwo(p.y - Y_HSEPARATOR))*(i+PASSO);
    // inizializzaizione delle informazioni dell'alieno
    aliens[idNumber].typeObject = ENEMY_SHIP_TYPE;
    aliens[idNumber].direction = DOWN_DIRECTION;
    aliens[idNumber].tid = pthread_self();
    aliens[idNumber].idObj = idNumber;
    aliens[idNumber].health = FULL;
    statusAliens[idNumber] = OBJ_ALIVE; 
    
    pthread_mutex_unlock(&mutex);
    while(true){
        /* SPOSTAMENTO SINCRONO DEGLI ALIENI */
        switch(aliens[idNumber].direction){
            case UP_DIRECTION:
                if(numSpostamenti == MIN_MOVE_ALIEN){
                    aliens[idNumber].direction = DOWN_DIRECTION;
                    aliens[idNumber].pos.x--;
                }else{
                    aliens[idNumber].pos.y--;
                }
                numSpostamenti--;
                break;
            case DOWN_DIRECTION:
                if(numSpostamenti == MAX_MOVE_ALIEN){
                    aliens[idNumber].direction = UP_DIRECTION;
                    aliens[idNumber].pos.x--;
                }else{
                    aliens[idNumber].pos.y++;
                }
                numSpostamenti++;                
                break; 
        }
        
        if(generateBomb && rand()%10 == DEFAULT_VALUE){
            pthread_create(&tNBombs[idNumber], NULL, bombController, idNumberT);
            pthread_detach(tNBombs[idNumber]);
            generateBomb = false;
        }

        // se lo stato è diverso da OBJ_ALIVE, significa che il figlio è uscito
        if(statusBombs[idNumber] == OBJ_DEAD){
            generateBomb = true; 
            statusBombs[idNumber] = DEFAULT_VALUE;
        }
        
        usleep(300000);
    }
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
void* bombController(void* idNumberT){
    pthread_mutex_lock(&mutex);
    int idNumber = *((int*) idNumberT);
    bombs[idNumber].pos.x = aliens[idNumber].pos.x-PASSO;
    bombs[idNumber].pos.y = aliens[idNumber].pos.y;
    bombs[idNumber].typeObject = BOMB_TYPE;
    bombs[idNumber].direction = UP_DIRECTION;
    bombs[idNumber].tid = pthread_self();
    statusBombs[idNumber] = OBJ_ALIVE;
    pthread_mutex_unlock(&mutex);
    while(bombs[idNumber].pos.x > 0){
        pthread_mutex_lock(&mutex);
        bombs[idNumber].pos.x--;
        pthread_mutex_unlock(&mutex);
        usleep(60000);
    }
    statusBombs[idNumber] = OBJ_DEAD;
}

/**
 * @brief 
 * 
 * @param win 
 * @param p 
 * @param pipeIn 
 * @param difficultyMode 
 */
EndGame printObjects () {
    int i, j, nAliensAlive = FULL;
    usleep(10000);
    do {  
        checkCollision();
        gameStatus = isGameOver(nAliensAlive);
        if(gameStatus == CONTINUE){
            hudGame(allyShipBuffer->health, nAliensAlive);
            printStarShip(*allyShipBuffer);

            for(i=0;i<NUMBER_BULLETS;i++){
                if(statusBullets[i] == OBJ_ALIVE){
                    printBullet(bullets[i]);
                }
            }

            for(i=0;i<numAliens;i++){
                if(statusAliens[i] == OBJ_ALIVE){
                    printStarShip(aliens[i]);
                    
                }
                //mvwprintw(winT, i, 3, "bomb: %d", statusBombs[i]);
                if(statusBombs[i] == OBJ_ALIVE){
                    printBullet(bombs[i]);
                }
            }

            for(i=Y_HSEPARATOR;i<p.y;i++){
                mvwaddch(winT, i, 0, BLANK_SPACE);
                mvwaddch(winT, i, p.x-1, BLANK_SPACE);
            }

            wmove(winT, Y_HSEPARATOR, 0);
            whline(winT, ACS_HLINE, p.x);

            wrefresh(winT);
        }
        if(firstAlienKilled){
            nAliensAlive = countObjects(aliens, numAliens);
        }
    } while (gameStatus == CONTINUE);
    
    return gameStatus;
}

void checkCollision(){
    
    int i,j,k;
    
    //check navicella alleata
    for(i=0; i<numAliens; i++){
        if(statusBombs[i] == OBJ_ALIVE && checkAllyBombCollision(allyShipBuffer->pos, bombs[i].pos)){ //collisione bomba con allyship
            pthread_mutex_lock(&mutex);
            allyShipBuffer->health--;
            killThread(tNBombs, statusBombs, i);
            clearObjects(winT, p, bombs[i]);
            pthread_mutex_unlock(&mutex);
        }
        if(statusAliens[i] == OBJ_ALIVE && checkAllyAlienCollision(aliens[i].pos)){ //collisione tra navicella alleata e alieno
            gameStatus = DEFEAT;
        }
    }

    // check navicella nemica
    for(i=0;i<numAliens;i++){
        if(statusAliens[i] == OBJ_ALIVE){
            for(j=0;j<NUMBER_BULLETS;j++){
                if(statusBullets[j] == OBJ_ALIVE && checkAlienBulletCollision(aliens[i].pos, bullets[j].pos)){
                    pthread_mutex_lock(&mutex);
                    killThread(tNBullets, statusBullets, j);
                    clearObjects(winT, p, bullets[j]);
                    aliens[i].health--;
                    initializeObject(bullets[j]);
                    if(aliens[i].health == NO_HEALTH_REMAINING){
                        killThread(tNAliens, statusAliens, i);
                        clearObjects(winT, p, aliens[i]);
                        aliens[i].tid = UNDEFINED_TID;
                        firstAlienKilled = 1;
                    } 
                    pthread_mutex_unlock(&mutex);
                }
            }
        }
    }
    
}

/**
 * @brief Procedura che ha la funzione di stampare la navicella alleata e gli alieni di primo
 * e secondo livello
 * 
 * @param win 
 * @param p 
 * @param ship 
 */
void printStarShip (Object ship) {
    int i, j, y;
    switch(ship.typeObject){
        /* STAMPA NAVICELLA ALLEATA */
        case ALLY_SHIP_TYPE:
            pickColor(winT, PAIR_COLOR_ALLY_SHIP);
            char allySprite[ROWS_STARSHIP][COLS_STARSHIP] = STARSHIP;
            mvwprintw(winT, ship.pos.y-OUTER_STARSHIP, ship.pos.x, BLANK_SPACES_STARSHIP);
            mvwprintw(winT, ship.pos.y+OUTER_STARSHIP, ship.pos.x, BLANK_SPACES_STARSHIP);
            for(i=0;i<ROWS_STARSHIP;i++){
                for(j=0;j<COLS_STARSHIP;j++){
                    y = ship.pos.y-divideByTwo(STARSHIP_SIZE) + i;
                    mvwaddch(winT, y, ship.pos.x+j, allySprite[i][j]);
                }
            }
            turnOffColor(winT, PAIR_COLOR_ALLY_SHIP);
            break;
        case ENEMY_SHIP_TYPE:
            y = ship.direction == UP_DIRECTION ? OUTER_ALIEN*DISTANCE_FROM_CENTER_ALIEN : OUTER_ALIEN*(-DISTANCE_FROM_CENTER_ALIEN);
            mvwprintw(winT, ship.pos.y+y, ship.pos.x-OUTER_ALIEN, BLANK_SPACES_ALIEN);
            /* CANCELLA POSIZIONE PRECEDENTE DELL'ALIENO */
            for(i=0;i<ALIEN_SIZE+2;i++){
                mvwaddch(winT, ship.pos.y+i-2, ship.pos.x+DISTANCE_FROM_CENTER_ALIEN, BLANK_SPACE);
                mvwaddch(winT, ship.pos.y+i-2, ship.pos.x+DISTANCE_FROM_CENTER_ALIEN+PASSO, BLANK_SPACE);
                mvwaddch(winT, ship.pos.y-DISTANCE_FROM_CENTER_ALIEN, ship.pos.x+i-2, BLANK_SPACE);
                mvwaddch(winT, ship.pos.y+DISTANCE_FROM_CENTER_ALIEN, ship.pos.x+i-2, BLANK_SPACE);
            }
            char alienSpriteBigEnemy[ROWS_ALIEN][COLS_ALIEN] = ENEMYSHIP_LEVEL_TWO;
            char alienSprite[ROWS_ALIEN][COLS_ALIEN] = ENEMYSHIP;
            
            for(i=0;i<ROWS_ALIEN;i++){
                for(j=0;j<COLS_ALIEN;j++){
                    y = ship.pos.y-divideByTwo(ALIEN_SIZE) + i;
                    switch(ship.health){
                        case FULL:
                            pickColor(winT, PAIR_COLOR_ALIEN);
                            mvwaddch(winT, y, ship.pos.x+j, alienSprite[i][j]);
                            turnOffColor(winT, PAIR_COLOR_ALIEN);
                            break;
                        case MEDIUM:
                            pickColor(winT, PAIR_COLOR_ALIEN_HALF_HEALTH);
                            mvwaddch(winT, y, ship.pos.x+j, alienSpriteBigEnemy[i][j]);
                            turnOffColor(winT, PAIR_COLOR_ALIEN_HALF_HEALTH);
                            break;
                        case LOW:
                            pickColor(winT, PAIR_COLOR_ALIEN_LOW_HEALTH);
                            mvwaddch(winT, y, ship.pos.x+j, alienSpriteBigEnemy[i][j]);
                            turnOffColor(winT, PAIR_COLOR_ALIEN_LOW_HEALTH);
                            break;
                    }
                }
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
void printBullet (Object bullet) {
    int y, x;
    switch (bullet.typeObject) {
        case BULLET_TYPE:
            pickColor(winT, PAIR_COLOR_BULLET);
            y = bullet.direction == UP_DIRECTION ? bullet.pos.y + PASSO : bullet.pos.y - PASSO;
            x = bullet.pos.x - BULLET_PACE; 
            mvwaddch(winT, y, x, BLANK_SPACE);
            mvwaddch(winT, bullet.pos.y, bullet.pos.x, BULLET_SPRITE);
            turnOffColor(winT, PAIR_COLOR_BULLET);
            break;
        case BOMB_TYPE:
            pickColor(winT, PAIR_COLOR_BOMB);
            y = bullet.pos.y;
            x = bullet.pos.x + PASSO;
            mvwaddch(winT, y, x, BLANK_SPACE);
            mvwaddch(winT, bullet.pos.y, bullet.pos.x, BOMB_SPRITE);
            turnOffColor(winT, PAIR_COLOR_BOMB);
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
void moveAllyShip () {
    int y = allyShipBuffer->pos.y;
    
    keypad(winT, TRUE);
    cbreak();
    wtimeout(winT, 100);
    int arrow = wgetch(winT);
    if(arrow == ASCII_CODE_SPACE_BAR) {
        isBulletShot = true;
    }
    pthread_mutex_lock(&mutex);
    keyPadSelector(winT, Y_HSEPARATOR+STARSHIP_SIZE, p.y-STARSHIP_SIZE, arrow, &allyShipBuffer->pos.y);
    pthread_mutex_unlock(&mutex);
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
    return difficultyMode == EASY ? FULL : LOW;
}

/**
 * @brief Funzione che determina la condizione di vittoria, sconfitta o di continuo del gioco
 * @param nAliensAlive 
 * @return EndGame 
 */
EndGame isGameOver (int nAliensAlive){
    /* gameStatus potrebbe essere gia' settata a sconfitta, in quanto la @checkCollision setta la 
        variabile globale a "DEFEAT" in caso di collisione */
    if(gameStatus == DEFEAT || allyShipBuffer->health == 0){ 
        return DEFEAT;
    }
    
    return nAliensAlive == DEFAULT_VALUE ? VICTORY : CONTINUE;
}

/**
 * @brief 
 * 
 */
void initializeBuffers(){
    allyShipBuffer = calloc(NUMBER_ALLYSHIP, sizeof(Object));
    bullets = calloc(NUMBER_BULLETS, sizeof(Object));
    aliens = calloc(numAliens, sizeof(Object));
    bombs = calloc(numAliens, sizeof(Object));
    statusBullets = calloc(NUMBER_BULLETS, sizeof(int));
    statusAliens = calloc(numAliens, sizeof(int));
    statusBombs = calloc(numAliens, sizeof(int));
    tNAliens = calloc(numAliens, sizeof(pthread_t));
    tNBombs = calloc(numAliens, sizeof(pthread_t));
    tNBullets = calloc(NUMBER_BULLETS, sizeof(pthread_t));
}

void killThread(pthread_t* thread, int* statusBuffer, int id){
    pthread_cancel(thread[id]);
    statusBuffer[id] = OBJ_DEAD;
    thread[id] = DEFAULT_VALUE;
}

void initializeObject(Object obj){
    obj.pos.x = DEFAULT_VALUE;
    obj.pos.y = DEFAULT_VALUE;
    obj.tid = UNDEFINED_TID;
    obj.typeObject= UNDEFINED;
    obj.idObj = DEFAULT_VALUE;
}

void freeAll(){
    free(bullets);
    free(aliens);
    free(allyShipBuffer);
    free(bombs);
    free(statusBullets);
    free(statusAliens);
    free(statusBombs);
}