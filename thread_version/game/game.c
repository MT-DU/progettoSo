#include "game.h"

// Dichiarazione delle variabili globali
Object* allyShipBuffer, *bullets, *aliens, *bombs; // Variabili di tipo Object usate come buffer
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; // Mutex per la sincronizzazione
pthread_t tAllyShip, *tNAliens, *tNBombs, *tNBullets; // Thread per la gestione dei thread
int firstAlienKilled, isBulletShot, isBombShot, canShoot, *statusBullets, *statusAliens, *statusBombs, numAliens; // Variabili di stato
WINDOW* winT; // Finestra di stampa
Point p; // Risoluzione della finestra
EndGame gameStatus = CONTINUE; // Variabile usata per controllare lo stato del gioco
DifficultyType difficulty; // Variabile che indica la difficoltà del gioco

/**
 * @brief Procedura principale da cui parte il gioco
 * 
 * @param win Finestra di stampa
 * @param difficultyMode Difficoltà del gioco
 */
void mainGame(WINDOW* win, DifficultyType difficultyMode){
    srand(time(NULL));
    int i, status;
    void* id = malloc(sizeof(void*));
    
    // Inizializzazione delle variabili globali
    winT = win;
    getmaxyx(winT, p.y, p.x);
    gameStatus = CONTINUE;
    difficulty = difficultyMode;
    numAliens = getMaxAlien(difficultyMode);
    int* arg = calloc(numAliens, sizeof(int*));
    initializeBuffers();
    statusBullets[UP_DIRECTION] = DEFAULT_VALUE;
    statusBullets[DOWN_DIRECTION] = DEFAULT_VALUE;
    firstAlienKilled = 0, isBulletShot = 0, isBombShot = 0, canShoot = 1;

    // Creazione dei thread e uso della detach per liberare la memoria in caso di terminazione del thread
    if(pthread_create(&tAllyShip, NULL, allyShipController, NULL)){
        printExceptions(TYPE_EXCEPTION_THREAD_CREATION_FAILURE);
    }
    pthread_detach(tAllyShip);
    for(i=0;i<numAliens;i++){
        pthread_mutex_lock(&mutex);
        arg[i] = i;
        pthread_mutex_unlock(&mutex);
        if(pthread_create(&tNAliens[i], NULL, enemyShipController, (void*) &arg[i])){
            printExceptions(TYPE_EXCEPTION_THREAD_CREATION_FAILURE);
        }
        
        pthread_detach(tNAliens[i]);
    }

    // Chiamata alla funzione che gestisce la stampa del gioco
    gameStatus = printObjects();
    
    // Unlock del mutex per la sincronizzazione in caso sia lockato
    if(pthread_mutex_trylock(&mutex) == 0){
        pthread_mutex_unlock(&mutex);
    }

    // Eliminazione del thread allyShip
    pthread_cancel(tAllyShip);
    
    // Eliminazione dei thread alieni e bombe
    for(i=0;i<numAliens;i++){
        if(statusAliens[i] == OBJ_ALIVE){
            killThread(tNAliens, statusAliens, i);
        }

        if(statusBombs[i] == OBJ_ALIVE){
            killThread(tNBombs, statusBombs, i);
        }        
    }

    // Eliminazione dei thread bullet
    for(i=0;i<NUMBER_BULLETS;i++){
        if(statusBullets[i] == OBJ_ALIVE){
            killThread(tNBullets, statusBullets, i);
        }
    }
    
    // Eliminazione del mutex
    pthread_mutex_destroy(&mutex);
    freeAll(); // Procedura che libera la memoria precedentemente allocata dinamicamente

    endGamePrint(winT, p, gameStatus);
    usleep(1000);
}

/**
 * @brief Procedura che visualizza l'hud del gioco
 * 
 * @param allyShipHealth Valore della vita della nave
 * @param nAliens Numero di alieni vivi
 */
void hudGame(int allyShipHealth, int nAliens){
    
    int i;
    int length = strlen(HEALTH_TEXT_HUD)+PASSO;
    int healthPosX = HEALTH_BAR_POS_X+length;
    
    // Stampa della vita della nave
    mvwprintw(winT, TEXT_HUD_POS_Y, HEALTH_BAR_POS_X, HEALTH_TEXT_HUD);
    mvwprintw(winT, TEXT_HUD_POS_Y, healthPosX, HEALTH_SPACE_HUD);
    pickColor(winT, PAIR_COLOR_HEART);
    for(i=0;i<allyShipHealth;i++){
        mvwprintw(winT, TEXT_HUD_POS_Y, healthPosX+i*3, "<3 ");
    }
    turnOffColor(winT, PAIR_COLOR_HEART);

    // Stampa del numero di alieni rimanenti
    pickColor(winT, PAIR_COLOR_ALIENS_REMAINING);
    mvwprintw(winT, TEXT_HUD_POS_Y, NUMBER_ALIENS_BAR_POS_X + strlen(ALIENS_TEXT_HUD)-1, BLANK_SPACES_ALIEN);
    mvwprintw(winT, TEXT_HUD_POS_Y, NUMBER_ALIENS_BAR_POS_X, ALIENS_TEXT_HUD, nAliens);
    turnOffColor(winT, PAIR_COLOR_ALIENS_REMAINING);
}

/**
 * @brief Procedura che gestisce la creazione e l'aggiornamento della navicella alleata
 * 
 */
void* allyShipController(){
    int i = 0;
    // Protezione della regione critica per la navicella alleata
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
        
        if(isBulletShot && canShoot){ // Controllo se la navicella alleata puo' sparare
            canShoot = false;

            for(i=0;i<NUMBER_BULLETS;i++){
                direction = i; 
                if(pthread_create(&tNBullets[i], NULL, bulletController, (void*) direction)){
                    printExceptions(TYPE_EXCEPTION_THREAD_CREATION_FAILURE);
                }
                pthread_detach(tNBullets[i]);
                
            } 
        }

        // Protezione della regione critica per la modifica delle variabili globali
        pthread_mutex_lock(&mutex);
        if(statusBullets[UP_DIRECTION] == OBJ_DEAD && statusBullets[DOWN_DIRECTION] == OBJ_DEAD){ // Controllo se i proiettili sono morti e riabilita la possibilita' di sparare
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
 + @param direction Indica la direzione del proiettile
 */
void* bulletController(void* directionV){
    Direction dir = (Direction) directionV;

    // Protezione della regione critica per il proiettile
    pthread_mutex_lock(&mutex);
    bullets[dir].pos.x = allyShipBuffer->pos.x + COLS_STARSHIP;
    bullets[dir].pos.y = allyShipBuffer->pos.y;
    bullets[dir].typeObject = BULLET_TYPE;
    bullets[dir].direction = ((Direction)directionV);
    bullets[dir].idObj = dir; // Assegno come id unico la direzione
    bullets[dir].tid = pthread_self();
    pthread_mutex_unlock(&mutex);
    statusBullets[dir] = OBJ_ALIVE;
    void* statusThread = malloc(sizeof(void*));

    while(p.x >= bullets[dir].pos.x){ // Ciclo finche' il proiettile non esce dallo schermo
        usleep(100000);
        switch (bullets[dir].direction) { // Gestione del movimento del proiettile e della meccanica del rimbalzo
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
 * @param idNumberT Identificativo dell'alieno
 */
void* enemyShipController (void* idNumberT) {
    int arg = *((int*) idNumberT);
    int statustid, i = 0, numSpostamenti = 3;
    bool generateBomb = true;

    // Protezione della regione critica per n-alieno
    pthread_mutex_lock(&mutex);
    int idNumber = *((int*) idNumberT);

    // Posizione iniziale dell'alieno con numero progressivo uguale a idNumber
    i = idNumber / MAX_ALIENS_FOR_ROW;
    aliens[idNumber].pos.x = p.x - (ALLY_BORDER_SPACE + OUTER_ALIEN + SPACE_BETWEEN_ALIENS)*(i+PASSO);
    i = idNumber % MAX_ALIENS_FOR_ROW;
    aliens[idNumber].pos.y = Y_HSEPARATOR + divideByTwo(divideByTwo(p.y - Y_HSEPARATOR))*(i+PASSO);

    // Inizializzaizione delle informazioni dell'alieno
    aliens[idNumber].typeObject = ENEMY_SHIP_TYPE;
    aliens[idNumber].direction = DOWN_DIRECTION;
    aliens[idNumber].tid = pthread_self();
    aliens[idNumber].idObj = idNumber;
    aliens[idNumber].health = FULL;
    statusAliens[idNumber] = OBJ_ALIVE; 
    
    pthread_mutex_unlock(&mutex);

    while(true){
        switch(aliens[idNumber].direction){ // Gestione del movimento dell'alieno
            case UP_DIRECTION:
                pthread_mutex_lock(&mutex);
                if(numSpostamenti == MIN_MOVE_ALIEN){
                    aliens[idNumber].direction = DOWN_DIRECTION;
                    aliens[idNumber].pos.x--;
                }else{
                    aliens[idNumber].pos.y--;
                }
                pthread_mutex_unlock(&mutex);
                numSpostamenti--;
                break;
            case DOWN_DIRECTION:
                pthread_mutex_lock(&mutex);
                if(numSpostamenti == MAX_MOVE_ALIEN){
                    aliens[idNumber].direction = UP_DIRECTION;
                    aliens[idNumber].pos.x--;
                }else{
                    aliens[idNumber].pos.y++;
                }
                pthread_mutex_unlock(&mutex);
                numSpostamenti++;                
                break; 
        }
        
        // Gestione della possibilita' di generare una bomba
        if(generateBomb && rand()%10 == DEFAULT_VALUE){
            if(pthread_create(&tNBombs[idNumber], NULL, bombController, idNumberT)){
                printExceptions(TYPE_EXCEPTION_THREAD_CREATION_FAILURE);
            }
            pthread_detach(tNBombs[idNumber]);
            generateBomb = false;
        }

        // Se la bomba e' morta, riabilita la possibilita' di generarla
        if(statusBombs[idNumber] == OBJ_DEAD){
            generateBomb = true; 
            statusBombs[idNumber] = DEFAULT_VALUE;
        }
        
        usleep(50000 * numAliens);
    }
}

/**
 * @brief Procedura che gestisce la generazione della bomba da parte della navicella nemica
 * 
 * @param idNumberT Identificativo della bomba
 */
void* bombController(void* idNumberT){

    // Protezione della regione critica per la bomba
    pthread_mutex_lock(&mutex);
    int idNumber = *((int*) idNumberT);
    bombs[idNumber].pos.x = aliens[idNumber].pos.x-PASSO;
    bombs[idNumber].pos.y = aliens[idNumber].pos.y;
    bombs[idNumber].typeObject = BOMB_TYPE;
    bombs[idNumber].direction = UP_DIRECTION;
    bombs[idNumber].tid = pthread_self();
    statusBombs[idNumber] = OBJ_ALIVE;
    pthread_mutex_unlock(&mutex);

    while(bombs[idNumber].pos.x > 0){ // Ciclo finche' la bomba non esce dallo schermo
        pthread_mutex_lock(&mutex);
        bombs[idNumber].pos.x--;
        pthread_mutex_unlock(&mutex);
        usleep(60000);
    }
    statusBombs[idNumber] = OBJ_DEAD;
}

/**
 * @brief Funzione che si occupa di stampare a schermo gli oggetti
 * 
 * @return EndGame Variabile che indica se l'utente ha vinto o meno
 */
EndGame printObjects () {
    int i, j, nAliensAlive = numAliens;
    usleep(10000);
    do {  
        checkCollision(); // Controllo se ci sono state delle collisioni
        wclear(winT);

        gameStatus = isGameOver(nAliensAlive); // Controllo se il gioco e' finito

        if(gameStatus == CONTINUE){ // Se il gioco e' ancora in corso, allora procedo con la stampa dell'hud e di tutti gli oggetti vivi
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

            // Ciclo che elimina eventuali proiettili o bombe morti
            for(i=Y_HSEPARATOR;i<p.y;i++){
                mvwaddch(winT, i, 0, BLANK_SPACE);
                mvwaddch(winT, i, p.x-1, BLANK_SPACE);
            }

            // Stampa dell'hud
            wmove(winT, Y_HSEPARATOR, 0);
            whline(winT, ACS_HLINE, p.x);
            hudGame(allyShipBuffer->health, nAliensAlive);

            wrefresh(winT);
        }
        if(firstAlienKilled){ // Se almeno un alieno e' morto, allora aggiorno il numero di alieni vivi
            nAliensAlive = countObjects(aliens, numAliens);
        }
    } while (gameStatus == CONTINUE);
    
    return gameStatus;
}

/**
 * @brief Procedura che leggendo dai buffer controlla se ci sono state collisioni tra oggetti ed elimina dallo schermo eventuali "cadaveri"
 * 
 */
void checkCollision(){
    
    int i,j,k; // Indici
    
    // Controllo delle collisioni che coinvolgono la navicella alleata
    for(i=0; i<numAliens; i++){
        if(statusBombs[i] == OBJ_ALIVE && checkAllyBombCollision(allyShipBuffer->pos, bombs[i].pos)){ // Collisione bomba con navicella alleata
            pthread_mutex_lock(&mutex);
            allyShipBuffer->health--;
            killThread(tNBombs, statusBombs, i);
            clearObjects(winT, p, bombs[i]);
            pthread_mutex_unlock(&mutex);
        }
        if(statusAliens[i] == OBJ_ALIVE && checkAllyAlienCollision(aliens[i].pos)){ // Collisione tra navicella alleata e alieno
            gameStatus = DEFEAT;
        }
    }

    // Controllo delle collisioni che coinvolgono la navicella nemica
    for(i=0;i<numAliens;i++){
        for(j=0;j<NUMBER_BULLETS;j++){
            if(statusBullets[j] == OBJ_ALIVE && statusAliens[i] == OBJ_ALIVE && checkAlienBulletCollision(aliens[i].pos, bullets[j].pos)){ // Collisione proiettile con alieno
                pthread_mutex_lock(&mutex);
                killThread(tNBullets, statusBullets, j);
                clearObjects(winT, p, bullets[j]);
                aliens[i].health--;
                initializeObject(bullets[j]);
                if(aliens[i].health == NO_HEALTH_REMAINING){ // Se l'alieno non ha piu' vita, lo elimino
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

/**
 * @brief Procedura che ha la funzione di stampare la navicella alleata, gli alieni di primo o secondo livello
 * 
 * @param ship Oggetto navicella/alieno da stampare
 */
void printStarShip (Object ship) {
    int i, j, y; // Indici

    switch(ship.typeObject){
        case ALLY_SHIP_TYPE: // Stampa navicella alleata e cancellazione della posizione precedente
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
        case ENEMY_SHIP_TYPE: // Stampa alieno e cancellazione della posizione precedente
            y = ship.direction == UP_DIRECTION ? OUTER_ALIEN*DISTANCE_FROM_CENTER_ALIEN : OUTER_ALIEN*(-DISTANCE_FROM_CENTER_ALIEN);
            mvwprintw(winT, ship.pos.y+y, ship.pos.x-OUTER_ALIEN, BLANK_SPACES_ALIEN);

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
                    switch(ship.health){ // Personalizzazione della stampa dell'alieno in base alla sua vita
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
 * @param bullet Oggetto proiettile/bomba da stampare
 */
void printBullet (Object bullet) {
    int y, x; // Indici

    switch (bullet.typeObject) {
        case BULLET_TYPE: // Stampa proiettile
            pickColor(winT, PAIR_COLOR_BULLET);
            y = bullet.direction == UP_DIRECTION ? bullet.pos.y + PASSO : bullet.pos.y - PASSO;
            x = bullet.pos.x - BULLET_PACE; 
            mvwaddch(winT, y, x, BLANK_SPACE);
            mvwaddch(winT, bullet.pos.y, bullet.pos.x, BULLET_SPRITE);
            turnOffColor(winT, PAIR_COLOR_BULLET);
            break;
        case BOMB_TYPE: // Stampa bomba
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
 */
void moveAllyShip () {
    int y = allyShipBuffer->pos.y;
    
    keypad(winT, TRUE);
    cbreak();
    wtimeout(winT, 100);
    int arrow = wgetch(winT);

    if(canShoot && arrow == ASCII_CODE_SPACE_BAR) { // Se viene premuto il tasto spazio e si è abilitati a sparare, viene attivata la flag dello sparo
        isBulletShot = true;
    }
    pthread_mutex_lock(&mutex);
    keyPadSelector(Y_HSEPARATOR+STARSHIP_SIZE, p.y-STARSHIP_SIZE, arrow, &allyShipBuffer->pos.y);
    pthread_mutex_unlock(&mutex);
    nocbreak();
}

/**
 * @brief Funzione che verifica se la posizione dell'oggetto passato e' nei limiti definiti dallo schermo
 * 
 * @param p Risoluzione dello schermo
 * @param yPos Posizione verticale dell'oggetto
 * @param size Dimensione dell'oggetto
 * @return true 
 * @return false 
 */
bool checkPos (Point p, int yPos, int size) {
    return yPos > Y_HSEPARATOR+size && yPos < p.y-size;
}


/**
 * @brief Funzione che restituisce il numero di alieni in base alla difficolta'
 * 
 * @param difficultyMode Variabile che indica la difficolta' del gioco
 * @return int 
 */
int getMaxAlien(DifficultyType difficultyMode){
    return difficultyMode.numAliens;
}

/**
 * @brief Funzione che restituisce la vita massima della navicella alleata in base alla difficolta'
 * 
 * @param difficultyMode Variabile che indica la difficolta' del gioco
 * @return int 
 */
int getMaxHealth(DifficultyType difficultyMode){
    switch(difficultyMode.type){
        case EASY:
            return FULL;
        case HARD:
            return LOW;
        case CUSTOM:
            return difficultyMode.numAliens > NUMBER_ENEMY_SHIPS_EASY ? LOW : FULL;
    }
}

/**
 * @brief Funzione che determina la condizione di vittoria, sconfitta o di continuo del gioco
 * 
 * @param nAliensAlive Numero di alieni attivi
 * @return EndGame 
 */
EndGame isGameOver (int nAliensAlive){
    /* gameStatus potrebbe essere gia' settata a sconfitta, in quanto la @checkCollision setta la 
        variabile globale a "DEFEAT" in caso di collisione tra navicella nemica e colonna a sinistra dello schermo */
    if(gameStatus == DEFEAT || allyShipBuffer->health == 0){ 
        return DEFEAT;
    }
    
    return nAliensAlive == DEFAULT_VALUE ? VICTORY : CONTINUE;
}

/**
 * @brief Procedura che inizializza tutte le variabili globali
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

/**
 * @brief Procedura che elimina un thread e setta le variabili globali ad un valore di default
 * 
 * @param thread Thread da eliminare
 * @param statusBuffer Buffer che contiene lo stato dei thread da reinizializzare
 * @param id id del thread da eliminare
 */
void killThread(pthread_t* thread, int* statusBuffer, int id){
    pthread_cancel(thread[id]);
    statusBuffer[id] = OBJ_DEAD;
    thread[id] = DEFAULT_VALUE;
}

/**
 * @brief Procedura che inizializza un oggetto con dei valori di default
 * 
 * @param obj Oggetto da inizializzare
 */
void initializeObject(Object obj){
    obj.pos.x = DEFAULT_VALUE;
    obj.pos.y = DEFAULT_VALUE;
    obj.tid = UNDEFINED_TID;
    obj.typeObject = UNDEFINED;
    obj.idObj = DEFAULT_VALUE;
}

/**
 * @brief Procedura che libera la memoria allocata per gli oggetti
 * 
 */
void freeAll(){
    free(bullets);
    free(aliens);
    free(allyShipBuffer);
    free(bombs);
    free(statusBullets);
    free(statusAliens);
    free(statusBombs);
}