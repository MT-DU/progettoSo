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
    int fileDes[DIM_PIPE], fileDesAliens[NUMBER_ENEMY_SHIPS][DIM_PIPE];
    int i;
    
    //initializePipe(fileDes);
    initializeNonBlockingPipe(fileDes);
    for(i = 0; i<NUMBER_ENEMY_SHIPS;i++){
        initializeNonBlockingPipe(fileDesAliens[i]);
    }
    switch (allyShip = fork()) { //creazione processo navicella alleata
        case PROCESS_RETURN_FAILURE:
            printExceptions(TYPE_EXCEPTION_PROCESS_CREATION_FAILURE);
            break;
        case PROCESS_RETURN_CHILD_PID: //processo figlio che gestisce la navicella alleata
            close(fileDes[PIPE_READ]);
            for(i = 0; i < NUMBER_ENEMY_SHIPS; i++){
                close(fileDesAliens[i][PIPE_READ]);
                close(fileDesAliens[i][PIPE_WRITE]);
            }
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
                        close(fileDes[PIPE_WRITE]);
                        close(fileDesAliens[i][PIPE_READ]);
                        enemyShipController(win, p, fileDesAliens[i][PIPE_WRITE], i);
                        break;                   
                }
            }    
            close(fileDes[PIPE_WRITE]);
            printObjects(win, p, fileDes[PIPE_READ], fileDesAliens);
        }
    kill(allyShip, SIGTERM);
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
    ship.pos.y = Y_HSEPARATOR + divideByTwo(p.y - Y_HSEPARATOR);
    ship.typeObject = ALLY_SHIP_TYPE;
    ship.direction = 0;
    ship.pid = getpid();
    ship.health = 3; //TODO cambiare in base alla difficolta'
    ship.nEntities = 1;
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
    alien.health=3;

    while(true){
        write(pipeOut, &alien, sizeof(Object));
       /*switch(alien.direction){
            case UP_DIRECTION:
                alien.pos.y--;
                if(numSpostamenti == 0){
                    alien.direction = DOWN_DIRECTION;
                    alien.pos.x--;
                }
                numSpostamenti--;
                break;
            case DOWN_DIRECTION:
                alien.pos.y++;
                if(numSpostamenti == 6){
                    perror("sesso che cade");
                    alien.direction = UP_DIRECTION;
                    alien.pos.x--;
                }
                numSpostamenti++;                
                break; 
        }*/
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
    perror("bomba smarties ke muore kadendo");
    _exit(SIGUSR2);
}

/**
 * @brief Procedura che gestisce la visualizzazione degli oggetti nella finestra
 * 
 * @param pipeOut 
 */
void printObjects (WINDOW* win, Point p, int pipeIn, int pipeAliens[NUMBER_ENEMY_SHIPS][DIM_PIPE]) {
    Object allyShip, aliens[NUMBER_ENEMY_SHIPS], obj;
    Object bullets[MAX_BULLETS_ACTIVE], bomb[NUMBER_ENEMY_SHIPS];
    objectArrayInitializer(bullets, MAX_BULLETS_ACTIVE);
    objectArrayInitializer(bomb, NUMBER_ENEMY_SHIPS);
    objectArrayInitializer(aliens, NUMBER_ENEMY_SHIPS);
    Status statoCollisione;
    int status;
    int i, contBulletsActive = 0; 
    while (true) {

        if(read(pipeIn, &obj, sizeof(Object)) > 0) { // lettura dalla pipe della navicella alleata
            switch(obj.typeObject){
                case ALLY_SHIP_TYPE:
                    allyShip = obj;
                    //mvwprintw(win, obj.pos.y+3, obj.pos.x, "O"); TODO se vuoi fixa senno sticazzi
                    //mvwprintw(win, obj.pos.y, obj.pos.x+7, "I");
                    printStarShip(win, allyShip);
                    break;
                case BULLET_TYPE:
                    addObject(bullets, MAX_BULLETS_ACTIVE, obj);
                    //catchZombies(bullets, MAX_BULLETS_ACTIVE, obj.pid);
                    printBullet(win, obj);
                    break;
            }      
        }
        
        for(i = 0; i<NUMBER_ENEMY_SHIPS; i++){  //ciclo tutte le pipe degli alieni
            if(read(pipeAliens[i][PIPE_READ], &obj, sizeof(Object)) > 0){   
                
                switch(obj.typeObject){  //switch del type object per la stampa
                    case ENEMY_SHIP_TYPE:
                        addObject(aliens, NUMBER_ENEMY_SHIPS, obj);
                        //mvwprintw(win, i, 0, "%d: (%d, %d) PID : %d", i,  aliens[i].pos.x, aliens[i].pos.y, aliens[i].pid);
                        // printStarShip(win, aliens[i]);
                        break;
                    case BOMB_TYPE:
                        mvwprintw(win, i, 0, "bomba numero %d: (%d, %d) PID : %d, objtype: %d, DIRECTION: %d", i,  obj.pos.x, obj.pos.y, obj.pid, obj.typeObject, obj.direction);
                        addObject(bomb, NUMBER_ENEMY_SHIPS, obj);
                        printBullet(win, obj);
                        break;
                }
            }
        }  

        //stampa degli oggetti nemici
        for(i = 0; i<NUMBER_ENEMY_SHIPS; i++){  //ciclo tutte le pipe degli alieni
            printStarShip(win, aliens[i]);
            //mvwprintw(win, i, 0, "Alieno numero %d: (%d, %d) PID : %d, objtype: %d, DIRECTION: %d", i,  aliens[i].pos.x, aliens[i].pos.y, aliens[i].pid, aliens[i].typeObject, aliens[i].direction);
            //printBullet(win, bomb[i]);
            //mvwprintw(win, (i+1)*2, 0, "bomba numero %d: (%d, %d) PID : %d, objtype: %d, DIRECTION: %d", i,  bomb[i].pos.x, bomb[i].pos.y, bomb[i].pid, bomb[i].typeObject, bomb[i].direction);

            // mvwprintw(win, aliens[i].pos.y-2, aliens[i].pos.x, "O"); TODO se vuoi fixa senno sticazzi
            // mvwprintw(win, aliens[i].pos.y, aliens[i].pos.x-10, "I");
        }
        
        //STAMPA RIGA ORIZZONTALE
        //wmove(win, Y_HSEPARATOR,0);
        //whline(win, ACS_HLINE,p.x);
        //mountainsBgEffect(win, p);
        statoCollisione = checkCollision(win, p, &allyShip, bomb, NUMBER_ENEMY_SHIPS);
        switch (statoCollisione.collision) {
            case LOSE_HEART_COLLISION:
                allyShip.health--;
                break;
            case DEATH_COLLISION:
                _exit(SIGINT);
                break;
        }
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
                            perror("Ceeee collisione balorda");
                        }
                        wrefresh(win);
                    }
                    break;
                case ENEMY_SHIP_TYPE:
                    for(i = 0; i<size; i++){
                        if(checkAllyAlienCollision(obj->pos, array->pos)){
                            // entrambi perdono una vita
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
                    for(i=0;i<size;i++){
                        if(checkAlienBulletCollision(obj->pos, array[i].pos)){
                            // uccidi bullet e decrementa vita alien di 1
                            
                        }
                    }
                    break;
                default:
                    perror("object colliding with alien is neither a bullet nor an alien");
                    break;
            }
            break;
        case BULLET_TYPE:
            if(arrayType == BOMB_TYPE) {
                for(i=0;i<size;i++) {
                    if(checkBulletBombCollision(obj->pos,array[i].pos)){
                        // uccidi entrambi i processi bullet e bomb
                    }
                }
            } else {
                perror("object colliding with bullet is not smarties bomb");
            }
            break;
        default:
            perror("wrong object type in checkCollision");
    }
    return status;
}

void checkWindowCollision(WINDOW* win, Point p, Object* obj){
    
    switch(obj->typeObject){
        case ENEMY_SHIP_TYPE:
            if(checkObjOutOfScreenLeft(obj->pos, ALIEN_SIZE)){
                // player perde una vita
            } else {
                if(checkObjOutOfScreenUpDown(p, obj->pos, ALIEN_SIZE)){
                    // fai rimbalzare (operatore ternario waiting room)
                }
            }
            break;
        case BULLET_TYPE:
            if(checkObjOutOfScreenRight(p, obj->pos, ALIEN_SIZE)){
                // processo missile muore
            } else {
                if(checkObjOutOfScreenUpDown(p, obj->pos, ALIEN_SIZE)){
                    // fai rimbalzare (operatore ternario waiting room again)
                }
            }
            break;
        case BOMB_TYPE:
            if(checkObjOutOfScreenLeft(obj->pos, ALIEN_SIZE)){
                // processo bomba muore 
            }
            break;
    }
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
    return yPos > Y_HSEPARATOR+size && yPos < p.y-size;
}