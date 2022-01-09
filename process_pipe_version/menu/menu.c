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
    Difficulty difficultyMode = EASY;
    keypad(win, true); 
    cbreak();
    do{
        wclear(win);
        hudMainMenu(win, max_res, PRINT_MENU_MAIN);
        printMenu(win, max_res, counter, NUMBER_CHOICES, vetLabelMenu);
        getInput = wgetch(win);
        keyPadSelector(PLAY_GAME_NUMBER, QUIT_GAME_NUMBER, getInput, &counter);
        selectOptionMainMenu(win, max_res, &getInput, counter, &difficultyMode);
        wrefresh(win);
    } while(getInput != ASCII_CODE_q && getInput != ASCII_CODE_Q); // q oppure Q sono i tasti per uscire
}

/**
 * @brief Procedura che stampa l'hud del menu principale
 * 
 * @param win Finestra su cui stampare l'hud
 * @param max_res Risoluzione della finestra
 */
void hudMainMenu(WINDOW *win, Point max_res, int menu){
    pickColor(win, PAIR_COLOR_LOGO);
    printLogo(win, max_res);
    wmove(win, Y_HLINE, MIN_POS);
    pickColor(win, PAIR_COLOR_TEXT);
    whline(win, ACS_HLINE, max_res.x);
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
void selectOptionMainMenu(WINDOW* win, Point max_res, int* input, int counter, Difficulty* difficultyMode){

    int i, y, x;
    int yDivided = divideByTwo(max_res.y), xDivided = divideByTwo(max_res.x);
    if((*input) == ASCII_CODE_ENTER){ // se l'utente ha premuto invio allora viene selezionata l'opzione
        switch (counter) {
            case PLAY_GAME_NUMBER: // seleziona la modalita' di gioco ed avvia il gioco
                wclear(win);
                tutorial(win, max_res);
                wclear(win);
                mainGame(win, max_res, *difficultyMode);
                fflush(stdin);
                break;
            case MODE_GAME_NUMBER: // seleziona la modalita' di gioco e richiama la procedura per modificare la modalita' di gioco
                wclear(win);
                hudMainMenu(win, max_res, PRINT_MENU_GAME_MODE);
                gameMode(win, max_res, difficultyMode);
                break;
            case QUIT_GAME_NUMBER: // esce dal gioco
                (*input) = ASCII_CODE_Q;
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

    int i = 7;

    while(i>0){
        hudMainMenu(win, max_res, PRINT_NO_MENU);

        mvwprintw(win, divideByTwo(max_res.y)-8, divideByTwo(max_res.x) - strlen(INTRO_1), INTRO_1);
        mvwprintw(win, divideByTwo(max_res.y)-6, divideByTwo(max_res.x) - strlen(INTRO_2), INTRO_2);
        mvwprintw(win, divideByTwo(max_res.y)-4, divideByTwo(max_res.x) - strlen(INTRO_3), INTRO_3);
        mvwprintw(win, divideByTwo(max_res.y)-2, divideByTwo(max_res.x) - strlen(INTRO_4), INTRO_4);
        mvwprintw(win, divideByTwo(max_res.y)  , divideByTwo(max_res.x) - strlen(INTRO_5), INTRO_5);
        mvwprintw(win, divideByTwo(max_res.y)+2, divideByTwo(max_res.x) - strlen(INTRO_6), INTRO_6);
        mvwprintw(win, divideByTwo(max_res.y)+4, divideByTwo(max_res.x) - strlen(INTRO_7), INTRO_7);
        mvwprintw(win, divideByTwo(max_res.y)+6, divideByTwo(max_res.x) - strlen(INTRO_8), INTRO_8);
        mvwprintw(win, divideByTwo(max_res.y)+8, divideByTwo(max_res.x) - strlen(INTRO_9), INTRO_9);

        mvwprintw(win, max_res.y-2, SPACE_FROM_BORDER, INTRO_10);
        mvwprintw(win, max_res.y-2, max_res.x - SPACE_FROM_BORDER - strlen(INTRO_11)-2, INTRO_11);
        mvwprintw(win, max_res.y-2, max_res.x - SPACE_FROM_BORDER - 2, " %d", i);

        usleep(DELAY_TUTORIAL);
        i--;
        wrefresh(win);
    }
    
}

/**
 * @brief Procedura che permette di stampare a schermo un menu' per la scelta della difficolta' del gioco
 * 
 * @param win 
 * @param max_res 
 */
void gameMode (WINDOW* win, Point max_res, Difficulty* difficultyMode) {
    int i, getInput, counter = 0;
    bool isPicked = false;
    char vetLabelMenu[NUMBER_CHOICES][DIM_MAX_PRINT_MENU] = LABEL_GAME_MODE_MENU;
    do{
        printMenu(win, max_res, counter, NUMBER_CHOICES, vetLabelMenu);
        getInput = wgetch(win);
        keyPadSelector(EASY_MODE_NUMBER, CUSTOM_MODE_NUMBER, getInput, &counter);
        selectDifficulty(win, max_res, getInput, counter, difficultyMode, &isPicked);
        wrefresh(win);
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
void selectDifficulty(WINDOW* win, Point max_res, int input, int counter, Difficulty* difficultyMode, bool* isPicked){
    int i, y, x;
    int yDivided = divideByTwo(max_res.y), xDivided = divideByTwo(max_res.x);
    if(input == ASCII_CODE_ENTER){
        switch (counter) {
            case EASY:
                (*difficultyMode) = EASY;
                *isPicked = true;
                break;
            case HARD:
                (*difficultyMode) = HARD;
                *isPicked = true;
                break;
            case CUSTOM:
                (*difficultyMode) = CUSTOM;
                *isPicked = true;
                break;
        }
    }
}

