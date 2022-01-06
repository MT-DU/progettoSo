#include "game.h"

Object* allyShipBuffer;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_t tAllyShip, tPrint, tNAliens[NUMBER_ENEMY_SHIPS_EASY], tNBombs[NUMBER_ENEMY_SHIPS_EASY], tNBullets[MAX_BULLETS_ACTIVE];
int put = 0, get = 0, isModified = 0;

/**
 * @brief Procedura principale da cui parte il gioco
 * 
 * @param win 
 * @param p
 * @param difficultyMode
 */
void mainGame(WINDOW* win, Point p, Difficulty difficultyMode){
    srand(time(NULL));
    void* winT;
    int i, status;
    EndGame gameStatus;
    winT = (void*)win;
    
    pthread_create(&tPrint, NULL, printObjects, winT);
    pthread_create(&tAllyShip, NULL, allyShipController, winT);
    pthread_join(tPrint, NULL);
    pthread_join(tAllyShip, NULL);
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
void hudGame(WINDOW* win, Point p, int allyShipHealth, int timer, int nAliens){
    
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
    //mvwprintw(win, TEXT_HUD_POS_Y, TIMER_BAR_POS_X, "Timer: %d", timer);
    pickColor(win, PAIR_COLOR_ALIENS_REMAINING);
    mvwprintw(win, TEXT_HUD_POS_Y, NUMBER_ALIENS_BAR_POS_X, ALIENS_TEXT_HUD, nAliens);
    turnOffColor(win, PAIR_COLOR_ALIENS_REMAINING);
}

/**
 * @brief Procedura che gestisce la creazione e l'utilizzo della navicella alleata
 * 
 * @param pipeOut 
 */
void* allyShipController(void* arg){
    Point p;
    WINDOW* win = (WINDOW*)arg;
    getmaxyx(win, p.y, p.x);
    int i = 0;
    pthread_mutex_lock(&mutex);
    allyShipBuffer = malloc(sizeof(Object));
    allyShipBuffer->pos.x = ALLY_BORDER_SPACE;
    allyShipBuffer->pos.y = Y_HSEPARATOR + divideByTwo(p.y - Y_HSEPARATOR);
    allyShipBuffer->typeObject = ALLY_SHIP_TYPE;
    allyShipBuffer->direction = 0;
    allyShipBuffer->pid = pthread_self();
    pthread_mutex_unlock(&mutex);
    int isBulletShot = false, canShoot = true;
    int status1 = 0, status2 = 0;
    while (true) {
        usleep(15000);
        moveAllyShip(win, p, &isBulletShot);
        /*
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
        */ 
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
 * @brief Procedura che gestisce la visualizzazione degli oggetti nella finestra e le loro collisioni
 * 
 * @param win 
 * @param p 
 * @param pipeIn 
 * @param difficultyMode 
 */
void* printObjects (void* arg) {
    Object allyShip, aliens[NUMBER_ENEMY_SHIPS_HARD], obj;
    Object bullets[MAX_BULLETS_ACTIVE], bomb[NUMBER_ENEMY_SHIPS_HARD];
    Point p;
    WINDOW* win = (WINDOW*)arg;
    getmaxyx(win, p.y, p.x);
    int status, i, allyShipHealth = 3, aliensHealth[NUMBER_ENEMY_SHIPS_HARD], nAliensAlive = NUMBER_ENEMY_SHIPS_EASY;
    int timer = 0;
    bool takeHealth = false, alienAllyCollision = false, firstKill = false;
    EndGame gameStatus = CONTINUE;
    initializeArray(aliensHealth, NUMBER_ENEMY_SHIPS_HARD, MAX_HEALTH_ALIEN);
    wclear(win);
    do {  
        if(isModified){
            allyShip = *allyShipBuffer;
            allyShipBuffer->health = allyShipHealth;
            //hudGame(win, p, allyShip.health, timer, nAliensAlive);
            printStarShip(win, p, *allyShipBuffer);
            wrefresh(win);
        }
    } while (gameStatus == CONTINUE);
    return (void*) gameStatus;
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
void moveAllyShip (WINDOW* win, Point p, int* isBulletShot) {
    int y = allyShipBuffer->pos.y;
    
    keypad(win, TRUE);
    cbreak();
    wtimeout(win, 0);
    int arrow = wgetch(win);
    if(arrow == ASCII_CODE_SPACE_BAR) {
        *isBulletShot = true;
    }
    pthread_mutex_lock(&mutex);
    keyPadSelector(win, Y_HSEPARATOR+STARSHIP_SIZE, p.y-STARSHIP_SIZE, arrow, &allyShipBuffer->pos.y);
    if(allyShipBuffer->pos.y == y){
        isModified = 0;
    }else{
        isModified = 1;
    }
    pthread_mutex_unlock(&mutex);    
    mvwprintw(win, 1, 50, "%d", isModified);
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
/*
void writeIntoBuffer(Object newObj){
    pthread_mutex_lock(&mutex);

    buffer[put] = newObj;
    put = (put + 1) % BUFFER_SIZE;

    pthread_mutex_unlock(&mutex);
}

Object readFromBuffer(){
    Object obj;
    pthread_mutex_lock(&mutex);

    obj = buffer[get];
    get = (get + 1) % BUFFER_SIZE;
    
    pthread_mutex_unlock(&mutex);
    return obj;
}*/

/**
 * @brief Procedura che si occupa di incrementare o decrementare la variabile counter in base all'input delle frecce direzionali
 * 
 * @param win 
 * @param min_choice valore minimo 
 * @param max_choice valore massimo
 * @param input 
 * @param counter puntatore alla variabile da gestire
 */
void keyPadSelectorThread(WINDOW *win, int min_choice, int max_choice, int input, int* counter){
    switch (input){
        case KEY_UP:
            if((*counter)>min_choice){
                pthread_mutex_lock(&mutex);
                (*counter)--;
                isModified = 1;
                pthread_mutex_unlock(&mutex);

            }
            break;
        case KEY_DOWN:
            if((*counter)<max_choice){
                pthread_mutex_lock(&mutex);
                (*counter)++;
                isModified = 1;
                pthread_mutex_unlock(&mutex);

            }
    }
}