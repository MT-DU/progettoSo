#include "game.h"

/**
 * @brief Procedura principale da cui parte il gioco
 * 
 * @param win Finestra su cui disegnare il gioco
 * @param p Struttura che contiene la risoluzione della finestra 
 * @param difficultyMode Parametro che indica la difficolta' del gioco
 */
void mainGame(WINDOW* win, Point p, DifficultyType difficultyMode){
    srand(time(NULL));
    pid_t pidEnemyShips[MAX_ALIENS], allyShip;
    int fileDes[DIM_PIPE];
    int i, status;
    EndGame gameStatus;

    initializePipe(fileDes); // Inizializzazione della pipe
    switch (allyShip = fork()) { // Creazione processo navicella alleata
        case PROCESS_RETURN_FAILURE:
            printExceptions(TYPE_EXCEPTION_PROCESS_CREATION_FAILURE);
            break;
        case PROCESS_RETURN_CHILD_PID: //processo figlio che gestisce la navicella alleata
            close(fileDes[PIPE_READ]);
            allyShipController(win, p, fileDes[PIPE_WRITE]);
            break;
        default:
            usleep(500);
            for(i = 0; i < getMaxAlien(difficultyMode); i++){ // cicla finche' non genera tutti gli alieni
                switch (pidEnemyShips[i] = fork()) { // Creazione processi alieni
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
            endGame(pidEnemyShips, allyShip, difficultyMode); // Chiusura processi
    }
    endGamePrint(win, p, gameStatus); // Stampa messaggio di fine gioco
    while(wait(0)>0);
}

/**
 * @brief Procedura che visualizza l'hud del gioco nella window
 * 
 * @param win Finestra su cui disegnare il gioco
 * @param p Struttura che contiene la risoluzione della finestra 
 * @param allyShipHealth Variabile che indica la vita della navicella alleata
 * @param nAliens Numero di alieni attualmente vivi nel gioco
 */
void hudGame(WINDOW* win, Point p, int allyShipHealth, int nAliens){
    
    int i;
    int length = strlen(HEALTH_TEXT_HUD)+PASSO;
    int healthPosX = HEALTH_BAR_POS_X+length;
    
    // Stampa della vita 
    mvwprintw(win, TEXT_HUD_POS_Y, HEALTH_BAR_POS_X, HEALTH_TEXT_HUD);
    mvwprintw(win, TEXT_HUD_POS_Y, healthPosX, HEALTH_SPACE_HUD);
    pickColor(win, PAIR_COLOR_HEART);
    for(i=0;i<allyShipHealth;i++){
        mvwprintw(win, TEXT_HUD_POS_Y, healthPosX+i*3, "<3 ");
    }
    turnOffColor(win, PAIR_COLOR_HEART);

    // Stampa numero alieni rimanenti
    pickColor(win, PAIR_COLOR_ALIENS_REMAINING);
    mvwprintw(win, TEXT_HUD_POS_Y, NUMBER_ALIENS_BAR_POS_X + strlen(ALIENS_TEXT_HUD)-1, BLANK_SPACES_ALIEN);
    mvwprintw(win, TEXT_HUD_POS_Y, NUMBER_ALIENS_BAR_POS_X, ALIENS_TEXT_HUD, nAliens);
    turnOffColor(win, PAIR_COLOR_ALIENS_REMAINING);
}

/**
 * @brief Procedura che gestisce la creazione e l'utilizzo della navicella alleata
 * 
 * @param win Finestra su cui disegnare il gioco
 * @param p Struttura che contiene la risoluzione della finestra 
 * @param pipeOut Descriptor della pipe su cui scrivere i dati
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
    int isBulletShot = false, canShoot = true; // Variabili di controllo per evitare che la navicella alleata abbia piu' di una coppia di proiettili alla volta
    int status1 = 0, status2 = 0;

    while (true) {
        while(write(pipeOut, &ship, sizeof(Object)) == -1);
        moveAllyShip(win, p, &ship.pos.y, &isBulletShot);
        if(isBulletShot && canShoot){
            switch(bullets[UP_DIRECTION] = fork()){ // Creazione processo proiettile superiore
                case PROCESS_RETURN_FAILURE:
                    printExceptions(TYPE_EXCEPTION_PROCESS_CREATION_FAILURE);
                    break;
                case PROCESS_RETURN_CHILD_PID:
                    bulletController(win, p, ship.pos, UP_DIRECTION, pipeOut);
                    break;
            }
            switch(bullets[DOWN_DIRECTION] = fork()){ // Creazione processo proiettile inferiore
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
        // se lo stato e' diverso da OBJ_ALIVE, significa che il figlio e' uscito
        if(status1 != OBJ_ALIVE && status2 != OBJ_ALIVE){ // Se entrambi i proiettili sono morti, allora la navicella alleata puo' sparare nuovamente
            canShoot = true; 
            isBulletShot = false;
        }      
    }
}

/**
 * @brief Procedura che gestisce la creazione e lo spostamento della navicella nemica
 * 
 * @param win Finestra su cui disegnare il gioco
 * @param p Struttura che contiene la risoluzione della finestra 
 * @param pipeOut Descriptor della pipe su cui scrivere i dati
 * @param idNumber numero progressivo univoco di ciascun alieno
 * @param difficultyMode Parametro che indica la difficolta' del gioco (in questo caso quanti alieni devono essere generati)
 */
void enemyShipController(WINDOW* win, Point p, int pipeOut, int idNumber, DifficultyType difficultyMode) {
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
        while(write(pipeOut, &alien, sizeof(Object)) == -1);
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
        // se lo stato e' diverso da OBJ_ALIVE, significa che il figlio e' uscito
        if(statusPid != OBJ_ALIVE){ 
            generateBomb = true; 
        }   
        usleep(getDelay(difficultyMode));
    }
}

/**
 * @brief Procedura che gestisce la generazione del proiettile da parte della navicella alleata
 * 
 * @param win Finestra su cui disegnare il gioco
 * @param p Struttura che contiene la risoluzione della finestra 
 * @param posShip Posizione della navicella alleata che spara il proiettile
 * @param direction Variabile che indica la direzione del proiettile (superiore o inferiore) 
 * @param pipeOut  Descriptor della pipe su cui scrivere i dati
 */
void bulletController(WINDOW* win, Point p, Point posShip, Direction direction, int pipeOut){
    Object bullet;
    bullet.pos.x = posShip.x + COLS_STARSHIP;
    bullet.pos.y = posShip.y;
    bullet.typeObject = BULLET_TYPE;
    bullet.direction = direction;
    bullet.idObj = direction; // assegno come id unico la direzione
    bullet.pid = getpid();
    while(p.x >= bullet.pos.x){ // Ciclo finche' il proiettile non e' fuori dalla finestra
        switch (bullet.direction) { // Switch che si occupa di gestire il rimbalzo del proiettile e cambia la direzione in caso collida con i bordi superiore e inferiore
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
            
        while(write(pipeOut, &bullet, sizeof(Object)) == -1);
        usleep(100000);
    }
    _exit(SIGUSR1);
}

/**
 * @brief Procedura che gestisce la generazione della bomba da parte della navicella nemica
 * 
 * @param win Finestra su cui disegnare il gioco
 * @param p Struttura che contiene la risoluzione della finestra 
 * @param posAlien Posizione dell'alieno che spara la bomba 
 * @param pipeOut Descriptor della pipe su cui scrivere i dati
 */
void bombController(WINDOW* win, Point p, Point posAlien, int pipeOut){
    Object bomb;
    bomb.pos.x = posAlien.x-PASSO;
    bomb.pos.y = posAlien.y;
    bomb.typeObject = BOMB_TYPE;
    bomb.direction = UP_DIRECTION;
    bomb.pid = getpid();
    while(bomb.pos.x > 0){ // Ciclo finche' la bomba non e' fuori dalla finestra
        bomb.pos.x--;
        usleep(50000);
        while(write(pipeOut, &bomb, sizeof(Object)) == -1);
    }
    _exit(SIGUSR2);
}

/**
 * @brief Procedura che gestisce la visualizzazione degli oggetti nella finestra e le loro collisioni
 * 
 * @param win Finestra su cui disegnare il gioco
 * @param p Struttura che contiene la risoluzione della finestra 
 * @param pipeIn Descriptor della pipe dalla quale leggere i dati 
 * @param difficultyMode Variabile che indica la difficolta' del gioco
 */
EndGame printObjects (WINDOW* win, Point p, int pipeIn, DifficultyType difficultyMode) {
    // Dichiarazione di variabili di supporto per la check delle collisioni
    Object allyShip, aliens[MAX_ALIENS], obj;
    Object bullets[NUMBER_BULLETS], bomb[MAX_ALIENS];
    objectArrayInitializer(bullets, NUMBER_BULLETS);
    objectArrayInitializer(bomb, MAX_ALIENS);
    objectArrayInitializer(aliens, MAX_ALIENS);
    int status, i, allyShipHealth = getMaxHealth(difficultyMode), aliensHealth[MAX_ALIENS], nAliensAlive = getMaxAlien(difficultyMode);
    bool takeHealth = false, alienAllyCollision = false, firstKill = false;
    EndGame gameStatus = CONTINUE;
    initializeArray(aliensHealth, MAX_ALIENS, MAX_HEALTH_ALIEN);
    wclear(win);
    do { // Ciclo che si occupa di gestire la visualizzazione degli oggetti e le loro collisioni
        usleep(500);
        if(read(pipeIn, &obj, sizeof(Object)) > 0) { // lettura dalla pipe della navicella alleata, controllando il valore di ritorno per evitare letture da buffer vuoto
            switch(obj.typeObject){
                case ALLY_SHIP_TYPE:
                    allyShip = obj;
                    allyShip.health = allyShipHealth;
                    printStarShip(win, p, allyShip);
                    break;
                case BULLET_TYPE:
                    bullets[obj.idObj] = obj;
                    for(i=0; i<getMaxAlien(difficultyMode); i++){ // Ciclo che controlla se il proiettile colpisce un alieno
                        if(aliens[i].pid != UNDEFINED_PID && checkAlienBulletCollision(aliens[i].pos, bullets[obj.idObj].pos)){ // In caso affermativo, il proiettile viene terminato e cancellato dallo schermo e l'alieno subisce danni
                            aliensHealth[i]--;
                            aliens[i].health = aliensHealth[i];
                            kill(bullets[obj.idObj].pid, SIGUSR1);
                            wclear(win);
                            bullets[obj.idObj].pid = UNDEFINED_PID;
                            clearObjects(win, p, bullets[obj.idObj]);
                            if(aliens[i].health == NO_HEALTH_REMAINING){ // Se l'alieno ha come vita rimanente 0, viene terminato e cancellato dallo schermo
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
                    
                    if(checkAllyAlienCollision(aliens[obj.idObj].pos)){ // Se la navicella nemica colpisce la navicella alleata, ovvero si trova nella colonna piu' a sinistra dello schermo, viene attivata una flag
                        alienAllyCollision=true;
                    }
                    break;
                case BOMB_TYPE:
                    bomb[obj.idObj] = obj;
                    if(checkAllyBombCollision(allyShip.pos, bomb[obj.idObj].pos)){ // Se la bomba colpisce la navicella alleata, la navicella alleata subisce danni
                        allyShipHealth--;
                        kill(bomb[obj.idObj].pid, SIGUSR2);
                        clearObjects(win, p, obj);
                    }
                    break;
            }      
        }        

        // Stampa dei proiettili vivi
        for(i = 0; i<NUMBER_BULLETS; i++){ 
            if(bullets[i].pid != UNDEFINED_PID){
                printBullet(win, bullets[i]);
            }
        }

        //stampa degli oggetti nemici (alieni e bombe)
        for(i = 0; i<getMaxAlien(difficultyMode); i++){  
            if(aliens[i].pid != UNDEFINED_PID){
                printStarShip(win, p, aliens[i]);
            }
            if(bomb[i].pid != UNDEFINED_PID){
                printBullet(win, bomb[i]);
            }
        }

        // Pulizia dello schermo da eventuali bombe che arrivano al bordo sinistro dello schermo
        for(i=Y_HSEPARATOR;i<p.y;i++){
            mvwaddch(win, i, 0, BLANK_SPACE);
        }
        
        // Se almeno un alieno e' morto, viene fatto il conteggio di quanti alieni sono rimasti vivi
        if(firstKill){
            nAliensAlive = countObjects(aliens, getMaxAlien(difficultyMode));
        }
        
        // Stampa dell'hud del gioco
        wmove(win, Y_HSEPARATOR, 0);
        whline(win, ACS_HLINE, p.x);
        hudGame(win, p, allyShip.health, nAliensAlive);

        // Refresh della variabile che controlla se e' finito il gioco
        gameStatus = isGameOver(allyShip.health, alienAllyCollision, nAliensAlive);

        wrefresh(win);
    } while (gameStatus == CONTINUE);
    return gameStatus;
}

/**
 * @brief Funzione che determina la condizione di vittoria, sconfitta o di continuo del gioco
 * 
 * @param healthAllyShip Vita della navicella alleata
 * @param alienAllyCollision Check se un alieno e' arrivato alla colonna piu' a sinistra dello schermo
 * @param nAliensAlive Numero di alieni rimasti vivi
 * @return EndGame 
 */
EndGame isGameOver (int healthAllyShip, bool alienAllyCollision, int nAliensAlive){
    if(healthAllyShip == 0 || alienAllyCollision){
        return DEFEAT;
    }
    
    return nAliensAlive == 0 ? VICTORY : CONTINUE;
}

/**
 * @brief Procedura che ha la funzione di stampare la navicella alleata e gli alieni di primo e secondo livello
 * 
 * @param win Finestra su cui disegnare il gioco
 * @param p Struttura che contiene la risoluzione della finestra 
 * @param ship Oggetto che rappresenta la navicella da stampare a schermo
 */
void printStarShip (WINDOW* win, Point p, Object ship) {
    int i,j, y;
    switch(ship.typeObject){
        case ALLY_SHIP_TYPE: // Stampa della navicella alleata e cancellazione della posizione precedente

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
        case ENEMY_SHIP_TYPE: // Stampa degli alieni e cancellazione della posizione precedente di ognuno
            y = ship.direction == UP_DIRECTION ? OUTER_ALIEN*DISTANCE_FROM_CENTER_ALIEN : OUTER_ALIEN*(-DISTANCE_FROM_CENTER_ALIEN);
            mvwprintw(win, ship.pos.y+y, ship.pos.x-OUTER_ALIEN, BLANK_SPACES_ALIEN);

            for(i=0;i<ALIEN_SIZE+2;i++){
                mvwaddch(win, ship.pos.y+i-2, ship.pos.x+DISTANCE_FROM_CENTER_ALIEN, BLANK_SPACE);
                mvwaddch(win, ship.pos.y+i-2, ship.pos.x+DISTANCE_FROM_CENTER_ALIEN+PASSO, BLANK_SPACE);
                mvwaddch(win, ship.pos.y-DISTANCE_FROM_CENTER_ALIEN, ship.pos.x+i-2, BLANK_SPACE);
                mvwaddch(win, ship.pos.y+DISTANCE_FROM_CENTER_ALIEN, ship.pos.x+i-2, BLANK_SPACE);
            }
            char alienSpriteBigEnemy[ROWS_ALIEN][COLS_ALIEN] = ENEMYSHIP_LEVEL_TWO;
            char alienSprite[ROWS_ALIEN][COLS_ALIEN] = ENEMYSHIP;
            
            for(i=0;i<ROWS_ALIEN;i++){
                for(j=0;j<COLS_ALIEN;j++){
                    y = ship.pos.y-divideByTwo(ALIEN_SIZE) + i;
                    switch(ship.health){ // Personalizzazione della stampa dell'alieno in base alla sua vita
                        case FULL:
                            pickColor(win, PAIR_COLOR_ALIEN);
                            mvwaddch(win, y, ship.pos.x+j, alienSprite[i][j]);
                            turnOffColor(win, PAIR_COLOR_ALIEN);
                            break;
                        case MEDIUM:
                            pickColor(win, PAIR_COLOR_ALIEN_HALF_HEALTH);
                            mvwaddch(win, y, ship.pos.x+j, alienSpriteBigEnemy[i][j]);
                            turnOffColor(win, PAIR_COLOR_ALIEN_HALF_HEALTH);
                            break;
                        case LOW:
                            pickColor(win, PAIR_COLOR_ALIEN_LOW_HEALTH);
                            mvwaddch(win, y, ship.pos.x+j, alienSpriteBigEnemy[i][j]);
                            turnOffColor(win, PAIR_COLOR_ALIEN_LOW_HEALTH);
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
 * @param win Finestra su cui disegnare il gioco
 * @param bullet Oggetto che rappresenta il proiettile o la bomba da stampare a schermo 
 */
void printBullet (WINDOW* win, Object bullet) {
    int y, x;
    switch (bullet.typeObject) {
        case BULLET_TYPE: // Stampa del proiettile
            pickColor(win, PAIR_COLOR_BULLET);
            y = bullet.direction == UP_DIRECTION ? bullet.pos.y + PASSO : bullet.pos.y - PASSO;
            x = bullet.pos.x - BULLET_PACE; 
            mvwaddch(win, y, x, BLANK_SPACE);
            mvwaddch(win, bullet.pos.y, bullet.pos.x, BULLET_SPRITE);
            turnOffColor(win, PAIR_COLOR_BULLET);
            break;
        case BOMB_TYPE: // Stampa della bomba
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
 * @param win Finestra su cui disegnare il gioco
 * @param p Struttura che contiene la risoluzione della finestra 
 * @param yPos Posizione verticale della navicella
 * @param isBulletShot Variabile usata per verificare se e' stato fatto uno sparo
 */
void moveAllyShip (WINDOW* win, Point p, int* yPos, int* isBulletShot) {
    cbreak();
    wtimeout(win, 1);
    int arrow = wgetch(win);
    if(arrow == ASCII_CODE_SPACE_BAR) {
        *isBulletShot = true;
    }
    keyPadSelector(Y_HSEPARATOR+STARSHIP_SIZE, p.y-STARSHIP_SIZE, arrow, yPos);
}

/**
 * @brief Funzione che verifica se la posizione dell'oggetto passato e' nei limiti definiti dallo schermo
 * 
 * @param p Struttura che contiene la risoluzione della finestra
 * @param yPos Posizione verticale dell'oggetto da verificare
 * @param size Grandezza dell'oggetto da verificare
 * @return true 
 * @return false 
 */
bool checkPos (Point p, int yPos, int size) {
    return yPos > Y_HSEPARATOR+size && yPos < p.y-size;
}

/**
 * @brief Procedura che termina correttamente i processi in caso di fine partita
 * 
 * @param aliens Vettore di processi degli alieni
 * @param allyShip Processo della navicella alleata
 * @param difficultyMode Variabile che indica la difficolta' del gioco (numero di alieni massimo)
 */
void endGame(pid_t aliens[], pid_t allyShip, DifficultyType difficultyMode){
    int i;
    for(i=0; i<getMaxAlien(difficultyMode); i++){
        kill(aliens[i], SIGUSR2);
    }
    kill(allyShip, SIGUSR2);
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
            return MAX_HEALTH_ALLY_EASY;
        case HARD:
            return MAX_HEALTH_ALLY_HARD;
        case CUSTOM:
            return difficultyMode.numAliens > NUMBER_ENEMY_SHIPS_EASY ? MAX_HEALTH_ALLY_HARD : MAX_HEALTH_ALLY_EASY;
    }
}

/**
 * @brief Funzione che restituisce il valore da usare nella usleep del processo alieno in base alla difficolta' (velocita' di movimento)
 * 
 * @param difficultyMode Variabile che indica la difficolta' del gioco
 * @return int 
 */
int getDelay(DifficultyType difficultyMode){
    return difficultyMode.type == EASY ? DELAY_ALIEN_EASY : DELAY_ALIEN_HARD;
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

        case VICTORY: // Stampa in caso di vittoria

            // Caricamento del messaggio di vittoria sulla variabile winPrint
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

            // Stampa del messaggio di vittoria
            for(i=0;i<CUP_ROWS;i++){
                mvwprintw(win, y+i-(CUP_COLS/2), x-(CUP_ROWS/2), winPrint[i]);
            }
            break;

        case DEFEAT: // Stampa in caso di sconfitta

            // Caricamento del messaggio di sconfitta sulla variabile defeatPrint
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

            // Stampa del messaggio di sconfitta
            for(i=0;i<FACE_ROWS;i++){
                mvwprintw(win, y+i-(FACE_ROWS/2), x-(FACE_COLS/2), defeatPrint[i]);
            }
            break;
    }
    
    // Stampa della voce che annuncia il ritorno alla schermata di menu e refresh della finestra
    i = 5;
    while(i>0){
        mvwprintw(win, p.y-1, divideByTwo(p.x) - divideByTwo(sizeof(BACK_TO_MAIN_MENU)), BACK_TO_MAIN_MENU, i);
        wrefresh(win);
        usleep(1000000);
        i--;
    }
}