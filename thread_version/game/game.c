#include "game.h"

Object* allyShipBuffer, *bullets, *aliens, *bombs;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_t tAllyShip, tPrint, tNAliens[NUMBER_ENEMY_SHIPS_EASY], tNBombs[NUMBER_ENEMY_SHIPS_EASY], tNBullets[MAX_BULLETS_ACTIVE];
int *isModified, isBulletShot = 0, isBombShot = 0, canShoot = 1, *statusBullets, *statusAliens, *statusBombs, numAliens;
WINDOW* winT;
Point p;


/**
 * @brief Procedura principale da cui parte il gioco
 * 
 * @param win 
 * @param p
 * @param difficultyMode
 */
void mainGame(WINDOW* win, Difficulty difficultyMode){
    srand(time(NULL));
    int i, status;
    EndGame gameStatus;
    winT = win;
    getmaxyx(winT, p.y, p.x);

    numAliens = getMaxAlien(difficultyMode);

    initializeBuffers();

    pthread_create(&tAllyShip, NULL, allyShipController, NULL);
    pthread_create(&tPrint, NULL, printObjects, NULL);
    pthread_join(tPrint, NULL);
    pthread_detach(tAllyShip);
    //pthread_join(tPrint, (void**) gameStatus);
    //pthread_join(tAllyShip, NULL);
}

/**
 * @brief Procedura che visualizza la hud nella window
 * 
 * @param win 
 * @param p 
 * @param allyShipHealth 
 * @param timer 
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
}

/**
 * @brief Procedura che gestisce la creazione e l'utilizzo della navicella alleata
 * 
 * @param pipeOut 
 */
void* allyShipController(){
    int i = 0;
    pthread_mutex_lock(&mutex);
    allyShipBuffer->pos.x = ALLY_BORDER_SPACE;
    allyShipBuffer->pos.y = Y_HSEPARATOR + divideByTwo(p.y - Y_HSEPARATOR);
    allyShipBuffer->typeObject = ALLY_SHIP_TYPE;
    allyShipBuffer->direction = 0;
    allyShipBuffer->pid = pthread_self();
    pthread_mutex_unlock(&mutex);
    Direction direction;
    while (true) {
        usleep(15000);
        moveAllyShip(winT);
        
        if(isBulletShot && canShoot){
            canShoot = false;
            statusBullets[UP_DIRECTION] = OBJ_ALIVE;
            statusBullets[DOWN_DIRECTION] = OBJ_ALIVE;
            mvwprintw(winT, 2, 50, "%d", statusBullets[UP_DIRECTION]);
            mvwprintw(winT, 3, 50, "%d", statusBullets[DOWN_DIRECTION]);
            for(i=0;i<NUMBER_BULLETS;i++){
                direction = i;
                pthread_mutex_lock(&mutex);
                pthread_create(&tNBullets[i], NULL, bulletController, (void*) direction);
                pthread_mutex_unlock(&mutex);
            } 
        }
        
        mvwprintw(winT, 2, 50, "%d", statusBullets[UP_DIRECTION]);
        mvwprintw(winT, 3, 50, "%d", statusBullets[DOWN_DIRECTION]);
        pthread_mutex_lock(&mutex);
        if(statusBullets[UP_DIRECTION] != OBJ_ALIVE && statusBullets[DOWN_DIRECTION] != OBJ_ALIVE){ 
            canShoot = true; 
            isBulletShot = false;
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
    bullets[dir].pos.x = allyShipBuffer->pos.x + COLS_STARSHIP;
    bullets[dir].pos.y = allyShipBuffer->pos.y;
    bullets[dir].typeObject = BULLET_TYPE;
    bullets[dir].direction = ((Direction)directionV);
    bullets[dir].idObj = dir; //assegno come id unico la direzione
    bullets[dir].pid = pthread_self();
    void* statusThread = malloc(sizeof(void*));
    while(p.x >= bullets[dir].pos.x){
        //pthread_mutex_lock(&mutex);
        switch (bullets[dir].direction) { 
            case UP_DIRECTION:
                if(checkPos(p, bullets[dir].pos.y, PASSO)){
                    bullets[dir].pos.y--;
                    bullets[dir].pos.x += BULLET_PACE;
                } else {
                    bullets[dir].direction = DOWN_DIRECTION;
                    bullets[dir].pos.y++;
                    bullets[dir].pos.x += BULLET_PACE;
                }
                break;
            case DOWN_DIRECTION:
                if(checkPos(p, bullets[dir].pos.y, PASSO)){
                    bullets[dir].pos.y++;
                    bullets[dir].pos.x += BULLET_PACE;
                } else {
                    bullets[dir].direction = UP_DIRECTION;
                    bullets[dir].pos.y--;
                    bullets[dir].pos.x += BULLET_PACE;
                }
            }
        //pthread_mutex_unlock(&mutex);
        usleep(100000);
    }
    statusBullets[dir] = OBJ_DEAD;
    //statusThread = (void*) OBJ_DEAD;
    pthread_exit(NULL);
}

/**
 * @brief Procedura che gestisce la visualizzazione degli oggetti nella finestra e le loro collisioni
 * 
 * @param win 
 * @param p 
 * @param pipeIn 
 * @param difficultyMode 
 */
void* printObjects () {
    Object allyShip, aliens[NUMBER_ENEMY_SHIPS_HARD], obj;
    Object bulletsPrint[MAX_BULLETS_ACTIVE], bomb[NUMBER_ENEMY_SHIPS_HARD];
    int status, i, allyShipHealth = 3, aliensHealth[NUMBER_ENEMY_SHIPS_EASY], nAliensAlive = NUMBER_ENEMY_SHIPS_EASY;
    int timer = 0;
    bool takeHealth = false, alienAllyCollision = false, firstKill = false;
    EndGame gameStatus = CONTINUE;
    initializeArray(aliensHealth, NUMBER_ENEMY_SHIPS_EASY, MAX_HEALTH_ALIEN);
    wclear(winT);
    do {  
        if(isModified[ALLY_SHIP_TYPE]){ // TODO isModified da chi può venir toccato?
            allyShip = *allyShipBuffer;
            allyShipBuffer->health = allyShipHealth; // TODO qui potrebbe servire un mutex
            //hudGame(win, p, allyShip.health, nAliensAlive);
            pthread_mutex_lock(&mutex);
            printStarShip(*allyShipBuffer);
            pthread_mutex_unlock(&mutex);
        }

        for(i=0;i<NUMBER_BULLETS;i++){
            if(statusBullets[i] == OBJ_ALIVE){
                pthread_mutex_lock(&mutex);
                bulletsPrint[i] = bullets[i];
                printBullet(bullets[i]);
                pthread_mutex_unlock(&mutex);
            }

        }
        wrefresh(winT);
    } while (gameStatus == CONTINUE);
    return NULL;
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
    wtimeout(winT, 0);
    int arrow = wgetch(winT);
    if(arrow == ASCII_CODE_SPACE_BAR) {
        isBulletShot = true;
    }
    pthread_mutex_lock(&mutex);
    keyPadSelector(winT, Y_HSEPARATOR+STARSHIP_SIZE, p.y-STARSHIP_SIZE, arrow, &allyShipBuffer->pos.y);
    isModified[ALLY_SHIP_TYPE] = allyShipBuffer->pos.y != y; //se e' stata modificata diventa vera, altrimenti diventa falsa
    pthread_mutex_unlock(&mutex);
    mvwprintw(winT, 1, 50, "%d", isModified[ALLY_SHIP_TYPE]);
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
 * @brief 
 * 
 */
void initializeBuffers(){
    allyShipBuffer = calloc(NUMBER_ALLYSHIP, sizeof(Object));
    bullets = calloc(NUMBER_BULLETS, sizeof(Object));
    aliens = calloc(numAliens, sizeof(Object));
    bombs = calloc(numAliens, sizeof(Object));
    isModified = calloc(NUMBER_ALLYSHIP, sizeof(int));
    statusBullets = calloc(NUMBER_BULLETS, sizeof(int));
    statusAliens = calloc(numAliens, sizeof(int));
    statusBombs = calloc(numAliens, sizeof(int));
}
