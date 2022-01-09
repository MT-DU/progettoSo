#include "menu.h"

int difficolta; // Variabile globale per la scelta della difficolta

/**
 * @brief Procedura iniziale del gioco. Vengono inizializzate le ncurses e la finestra di dialogo
 * 
 */
void mainApp(){
    initscr();
    noecho();
    curs_set(NO_CURSOR);
    
    setupColors();
    mainWindow();
    
    endwin();
}

/**
 * @brief Crea la finestra di dialogo e richiama la procedura per la creazione del menu.
 * 
 */
void mainWindow(){
    WINDOW* hud_win = newwin(0,0,0,0);
    Point max_res;
    getmaxyx(hud_win, max_res.y, max_res.x);
    mainMenu(hud_win, max_res);
}

/**
 * @brief Procedura che si occupa di stampare il main menu fin quando non si esce dal gioco.
 * 
 * @param win Finestra su cui stampare il menu'
 * @param max_res Risoluzione della finestra
 */
void mainMenu(WINDOW *win, Point max_res){
    int getInput, counter = 0;
    char vetLabelMenu[NUMBER_CHOICES][DIM_MAX_PRINT_MENU] = LABEL_MAIN_MENU;
    int isTutorialShowed = false;
    DifficultyType difficultyMode;
    difficultyMode.type = EASY;
    difficultyMode.numAliens = NUMBER_ENEMY_SHIPS_EASY;
    keypad(win, true); 
    cbreak();
    do{
        wclear(win);
        hudMainMenu(win, max_res, PRINT_MENU_MAIN, difficultyMode);
        printMenu(win, max_res, counter, NUMBER_CHOICES, vetLabelMenu);
        getInput = wgetch(win);
        keyPadSelector(PLAY_GAME_NUMBER, QUIT_GAME_NUMBER, getInput, &counter);
        selectOptionMainMenu(win, max_res, &getInput, counter, &difficultyMode, &isTutorialShowed);
        wrefresh(win);
    } while(getInput != ASCII_CODE_q && getInput != ASCII_CODE_Q); // q oppure Q sono i tasti per uscire
}

/**
 * @brief Procedura che stampa l'hud del menu principale
 * 
 * @param win Finestra su cui stampare l'hud
 * @param max_res Risoluzione della finestra
 */
void hudMainMenu(WINDOW *win, Point max_res, int menu, DifficultyType difficultyMode){
    pickColor(win, PAIR_COLOR_LOGO);
    printLogo(win, max_res);
    wmove(win, Y_HLINE, MIN_POS);
    pickColor(win, PAIR_COLOR_TEXT);
    whline(win, ACS_HLINE, max_res.x);
    difficultySelected(win, max_res, difficultyMode);
    switch (menu) {
    case PRINT_MENU_MAIN:
        mvwprintw(win, max_res.y-1, 0, EXIT_LABEL_HUD);
        break;
    case PRINT_MENU_GAME_MODE:
        mvwprintw(win, max_res.y-1, 0, BACK_LABEL_HUD);
        break;
    default:
        break;
    }
}

/**
 * @brief Stampa il logo del gioco
 * 
 * @param win Finestra su cui stampare il logo
 * @param max_res Risoluzione della finestra
 */
void printLogo (WINDOW* win, Point max_res) {
    int halfLenLogo = SIZE_LOGO / 2;
    int i;
    char logoCompleto[NUM_LOGHI][SIZE_LOGO] = {"","","",""};
    for(i = 0; i < NUM_LOGHI; i++){
        switch (i) {
            case 0:
                strcat(logoCompleto[i], LOGO1);
                break;
            case 1:
                strcat(logoCompleto[i], LOGO2);
                break;
            case 2:
                strcat(logoCompleto[i], LOGO3);
                break;
            case 3:
                strcat(logoCompleto[i], LOGO4);
                break;
        }
    }
    for (i = 0; i < NUM_LOGHI; i++) {
        mvwprintw(win, i+1, divideByTwo(max_res.x) - halfLenLogo, logoCompleto[i]);
    }
}

/**
 * @brief Procedura che gestisce la selezione dell'utente nel main menu'
 * 
 * @param win Finestra su cui stampare il menu'
 * @param max_res Risoluzione della finestra
 * @param input Input dell'utente
 * @param counter Opzione selezionata dall'utente
 * @param difficultyMode Difficolta modificabile dall'utente
 */
void selectOptionMainMenu(WINDOW* win, Point max_res, int* input, int counter, DifficultyType* difficultyMode, int* isTutorialShowed){

    int i, y, x;
    int yDivided = divideByTwo(max_res.y), xDivided = divideByTwo(max_res.x);
    if(*input == ASCII_CODE_ENTER){ // se l'utente ha premuto invio allora viene selezionata l'opzione
        switch (counter) {
            case PLAY_GAME_NUMBER: // seleziona la modalita' di gioco ed avvia il gioco
                wclear(win);
                if(!(*isTutorialShowed)){
                    tutorial(win, max_res);
                    wclear(win);
                    *isTutorialShowed = true;
                }
                mainGame(win, max_res, *difficultyMode);
                break;
            case MODE_GAME_NUMBER: // seleziona la modalita' di gioco e richiama la procedura per modificare la modalita' di gioco
                wclear(win);
                hudMainMenu(win, max_res, PRINT_MENU_GAME_MODE, *difficultyMode);
                gameMode(win, max_res, difficultyMode);
                break;
            case QUIT_GAME_NUMBER: // esce dal gioco
                *input = ASCII_CODE_Q;
                break;
        }
    }
}

/**
 * @brief Procedura che stampa a schermo una breve introduzione del gioco
 * 
 * @param win Finestra su cui stampare
 * @param max_res Risoluzione della finestra
 */
void tutorial (WINDOW* win, Point max_res) {
    do{
        mvwprintw(win, divideByTwo(max_res.y)-10, divideByTwo(max_res.x) - divideByTwo(strlen(INTRO_1)), INTRO_1);
        mvwprintw(win, divideByTwo(max_res.y)-8, divideByTwo(max_res.x) - divideByTwo(strlen(INTRO_2)), INTRO_2);
        mvwprintw(win, divideByTwo(max_res.y)-6, divideByTwo(max_res.x) - divideByTwo(strlen(INTRO_3)), INTRO_3);
        mvwprintw(win, divideByTwo(max_res.y)-4, divideByTwo(max_res.x) - divideByTwo(strlen(INTRO_4)), INTRO_4);
        mvwprintw(win, divideByTwo(max_res.y)-2, divideByTwo(max_res.x) - divideByTwo(strlen(INTRO_5)), INTRO_5);
        mvwprintw(win, divideByTwo(max_res.y)  , divideByTwo(max_res.x) - divideByTwo(strlen(INTRO_6)), INTRO_6);
        mvwprintw(win, divideByTwo(max_res.y)+2, divideByTwo(max_res.x) - divideByTwo(strlen(INTRO_7)), INTRO_7);
        mvwprintw(win, divideByTwo(max_res.y)+4, divideByTwo(max_res.x) - divideByTwo(strlen(INTRO_8)), INTRO_8);
        pickColor(win, PAIR_COLOR_LOGO);
        mvwprintw(win, max_res.y-4, divideByTwo(max_res.x) - divideByTwo(strlen(INTRO_9)), INTRO_9);
        turnOffColor(win, PAIR_COLOR_LOGO);
        mvwprintw(win, max_res.y-2, divideByTwo(max_res.x) - divideByTwo(strlen(INTRO_10)), INTRO_10);
        wtimeout(win, 100);
    }  while(wgetch(win)!=ASCII_CODE_ENTER);  
}

/**
 * @brief Procedura che permette di stampare a schermo un menu' per la scelta della difficolta' del gioco
 * 
 * @param win 
 * @param max_res 
 */
void gameMode (WINDOW* win, Point max_res, DifficultyType* difficultyMode) {
    int i, getInput, counter = 0;
    bool isPicked = false;
    char vetLabelMenu[NUMBER_CHOICES][DIM_MAX_PRINT_MENU] = LABEL_GAME_MODE_MENU;
    do{
        printMenu(win, max_res, counter, NUMBER_CHOICES, vetLabelMenu);
        getInput = wgetch(win);
        keyPadSelector(EASY_MODE_NUMBER, CUSTOM_MODE_NUMBER, getInput, &counter);
        selectDifficulty(win, max_res, getInput, counter, difficultyMode, &isPicked);
        wrefresh(win);
        // Se la difficolta e' stata selezionata, allora puo' uscire dal ciclo
    } while(getInput != ASCII_CODE_q && getInput != ASCII_CODE_Q && !isPicked);
}

/**
 * @brief Procedura che gestisce la scelta della difficolta' da parte dell'utente
 * 
 * @param win Finestra su cui stampare il menu'
 * @param max_res Risoluzione della finestra
 * @param input Input dell'utente
 * @param counter Opzione selezionata dall'utente
 * @param difficultyMode Difficolta scelta dall'utente
 * @param isPicked Indica se l'opzione e' stata selezionata
 */
void selectDifficulty(WINDOW* win, Point max_res, int input, int counter, DifficultyType* difficultyMode, bool* isPicked){
    int i, y, x;
    int yDivided = divideByTwo(max_res.y), xDivided = divideByTwo(max_res.x);
    if(input == ASCII_CODE_ENTER){
        switch (counter) {
            case EASY:
                difficultyMode->type = EASY;
                difficultyMode->numAliens = NUMBER_ENEMY_SHIPS_EASY;
                *isPicked = true;
                break;
            case HARD:
                difficultyMode->type = HARD;
                difficultyMode->numAliens = NUMBER_ENEMY_SHIPS_HARD;
                *isPicked = true;
                break;
            case CUSTOM:
                difficultyMode->type = CUSTOM;
                customDifficulty(win, max_res, difficultyMode);
                *isPicked = true;
        }
    }
}

/**
 * @brief Procedura che stampa a schermo l'attuale difficolta' del gioco
 * 
 * @param win Finestra su cui stampare
 * @param p Risoluzione della finestra
 * @param difficulty Difficolta' selezionata
 */
void difficultySelected(WINDOW* win, Point p, DifficultyType difficulty){
    switch (difficulty.type)
    {
    case EASY:
        mvwprintw(win, p.y - 1, p.x - strlen(EASY_MODE_SELECTED), EASY_MODE_SELECTED);
        break;
    case HARD:
        mvwprintw(win, p.y - 1, p.x - strlen(HARD_MODE_SELECTED), HARD_MODE_SELECTED);
        break;
    case CUSTOM:
        mvwprintw(win, p.y - 1, p.x - strlen(CUSTOM_MODE_SELECTED), CUSTOM_MODE_SELECTED);
        break;
    default:
        break;
    }
}

/**
 * @brief Procedura che permette di scegliere la difficolta' da parte dell'utente in modo personalizzato
 * 
 * @param win Finestra su cui stampare il menu'
 * @param max_res Risoluzione della finestra
 * @param difficultyMode Difficolta' scelta dall'utente
 */
void customDifficulty(WINDOW* win, Point max_res, DifficultyType* difficultyMode){
    int getInput;
    do{
        wclear(win);
        hudMainMenu(win, max_res, PRINT_MENU_GAME_MODE, *difficultyMode);
        printCustomMenu(win, max_res, difficultyMode->numAliens);
        getInput = wgetch(win);
        keyPadSelector(MIN_ALIENS, MAX_ALIENS, getInput, &difficultyMode->numAliens);
        wrefresh(win);
        // Se la difficolta e' stata selezionata, allora puo' uscire dal ciclo
        wtimeout(win, 10);
    } while(getInput != ASCII_CODE_ENTER);
}

/**
 * @brief Procedura che permette di stampare a schermo un menu' per la scelta della difficolta' del gioco
 * 
 * @param win Finestra su cui stampare il menu'
 * @param max_res Risoluzione della finestra
 * @param numAliens Numero di alieni da mostrare
 */
void printCustomMenu(WINDOW* win, Point max_res, int numAliens){
    int longestStringDivided = divideByTwo(strlen(CUSTOM_MODE_PRINT_HINT)), i;
    int yDivided = divideByTwo(max_res.y), xDivided = divideByTwo(max_res.x);
    int x = xDivided - divideByTwo(strlen(CUSTOM_MODE_PRINT));

    int x1, x2, y1, y2;
    x1 = xDivided - longestStringDivided - CUSTOM_MODE_NUMBER;
    x2 = xDivided + longestStringDivided + CUSTOM_MODE_NUMBER;
    y1 = yDivided - SPACE_BETWEEN_CHOICES - PASSO;
    y2 = yDivided + SPACE_BETWEEN_CHOICES + PASSO;
    mvwhline(win, y1, x1, 0, x2-x1);
    mvwhline(win, y2, x1, 0, x2-x1);
    mvwvline(win, y1, x1, 0, y2-y1);
    mvwvline(win, y1, x2, 0, y2-y1);
    mvwaddch(win, y1, x1, ACS_ULCORNER);
    mvwaddch(win, y2, x1, ACS_LLCORNER);
    mvwaddch(win, y1, x2, ACS_URCORNER);
    mvwaddch(win, y2, x2, ACS_LRCORNER);

    mvwprintw(win, yDivided - PASSO, xDivided - longestStringDivided, CUSTOM_MODE_PRINT_HINT);
    mvwprintw(win, yDivided + PASSO, x, CUSTOM_MODE_PRINT, numAliens);
}