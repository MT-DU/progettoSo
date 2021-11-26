#include "menu.h"

int difficolta;

/**
 * @brief Procedura iniziale del gioco.
 * Da qui si avviano i vari processi di gestione.
 * 
 */
void mainApp(){
    initscr();
    noecho();
    curs_set(NO_CURSOR);
    
    setupColors();
    mainWindow();
    
    refresh();
    endwin();
}

/**
 * @brief Crea la finestra di dialogo
 * 
 */
void mainWindow(){
    WINDOW* hud_win = newwin(0,0,0,0);
    Point max_res;
    getmaxyx(hud_win, max_res.y, max_res.x);
    mainMenu(hud_win, max_res);
}

/**
 * @brief Procedura che si occupa di stampare il main menu fin quanto non si quitta dal gioco
 * 
 * @param win 
 * @param max_res 
 */
void mainMenu(WINDOW *win, Point max_res){
    int getInput, counter = 0;
    char vetLabelMenu[NUMBER_CHOICES][DIM_MAX_PRINT_MENU] = LABEL_MAIN_MENU;
    keypad(win, true); 
    cbreak();
    do{
        wclear(win);
        hudMainMenu(win, max_res, PRINT_MENU_MAIN);
        printMenu(win, max_res, counter, PRINT_MENU_MAIN, NUMBER_CHOICES, vetLabelMenu);
        getInput = wgetch(win);
        keyPadSelector(win, PLAY_GAME_NUMBER, QUIT_GAME_NUMBER, getInput, &counter);
        selectOptionMainMenu(win, max_res, &getInput, counter);
        refresh();
    } while(getInput != ASCII_CODE_q && getInput != ASCII_CODE_Q); // q è il tasto per quittare
}

/**
 * @brief Procedura che stampa l'hud del menu principale
 * 
 * @param win 
 * @param max_res
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
 * @param win 
 * @param max_res.x 
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
 * @brief Procedura che gestisce la selezione dell'utente
 * 
 * @param win window attiva
 * @param max_res 
 * @param input
 * @param counter opzione selezionata dal main menu
 */
void selectOptionMainMenu(WINDOW* win, Point max_res, int* input, int counter){

    int i, y, x;
    int yDivided = divideByTwo(max_res.y), xDivided = divideByTwo(max_res.x);

    if((*input) == ASCII_CODE_ENTER){
        switch (counter) {
            case PLAY_GAME_NUMBER:
                wclear(win);
                mainGame(win, max_res);
                break;
            case MODE_GAME_NUMBER:
                wclear(win);
                hudMainMenu(win, max_res, PRINT_MENU_GAME_MODE);
                gameMode(win, max_res);
                break;
            case QUIT_GAME_NUMBER:
                (*input) = ASCII_CODE_Q;
                break;
        }
    }
}

/**
 * @brief Procedura che si occupa di gestire la scelta della difficolta' del gioco
 * 
 * @param win 
 * @param max_res 
 */
void gameMode (WINDOW* win, Point max_res) {
    int i, getInput, counter = 0;
    char vetLabelMenu[NUMBER_CHOICES][DIM_MAX_PRINT_MENU] = LABEL_GAME_MODE_MENU;
    do{
        printMenu(win, max_res, counter, PRINT_MENU_GAME_MODE, NUMBER_CHOICES, vetLabelMenu);
        getInput = wgetch(win);
        keyPadSelector(win, EASY_MODE_NUMBER, CUSTOM_MODE_NUMBER, getInput, &counter);
        refresh();
    } while(getInput != ASCII_CODE_q && getInput != ASCII_CODE_Q);
}