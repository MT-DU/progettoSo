#include "menu.h"
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
    WINDOW* hud_win = newwin(0,0,0,0); // dovrebbe essere fullscreen
    Point max_res;
    max_res.y = getmaxy(hud_win);
    max_res.x = getmaxx(hud_win);
    mainMenu(hud_win, max_res);
}

void mainMenu(WINDOW *win, Point max_res){
    int getInput, counter = 0;
    keypad(win, true); 
    cbreak();
    do{
        hudMainMenu(win, max_res);
        printMainMenu(win, max_res, counter);
        getInput = wgetch(win);
        keyPadSelector(win, PLAY_GAME_NUMBER, QUIT_GAME_NUMBER, getInput, &counter);
        selectOptionMainMenu(win, max_res, &getInput, counter);
        refresh();
    }while(getInput != ASCII_CODE_Q); // q è il tasto per quittare
}

/**
 * @brief Procedura che stampa l'hud del menu principale
 * 
 * @param win 
 * @param max_res
 */
void hudMainMenu(WINDOW *win, Point max_res){
    pickColor(win, PAIR_COLOR_LOGO);
    printLogo(win, max_res);
    wmove(win, Y_HLINE, MIN_POS);
    pickColor(win, PAIR_COLOR_TEXT);
    whline(win, ACS_HLINE, max_res.x);
}

/**
 * @brief Questa procedura ha lo scopo di stampare le voci del menu
 * 
 * @param win window attiva
 * @param max_res
 * @param selection opzione selezionata dal main menu
 */
void printMainMenu(WINDOW *win, Point max_res, int selection){
    int i, y, x;
    int yDivided = divideByTwo(max_res.y), xDivided = divideByTwo(max_res.x), modeGameDivided = divideByTwo(sizeof(MODE_GAME));
    for(i = 0; i <= QUIT_GAME_NUMBER; i++){ //blocco di codice per rimuovere i '>' dal menu
        y = yDivided + SPACE_BETWEEN_CHOICES * i;
        x = xDivided - modeGameDivided - SPACE_BETWEEN_CHOICES;
        mvwprintw(win, y, x, " ");
    }

    //riga di codice che aggiunge un '>' affianco all'opzione selezionata
    y = yDivided + SPACE_BETWEEN_CHOICES * selection;
    x = xDivided - modeGameDivided - SPACE_BETWEEN_CHOICES;
    mvwprintw(win, y, x, SELECTOR);

    x = xDivided - divideByTwo(sizeof(PLAY_GAME));
    mvwprintw(win, yDivided, x, PLAY_GAME);

    y = yDivided + SPACE_BETWEEN_CHOICES;
    x = xDivided - modeGameDivided;
    mvwprintw(win, y, x, MODE_GAME);
    
    y = yDivided + SPACE_BETWEEN_CHOICES*QUIT_GAME_NUMBER;
    x = xDivided - divideByTwo(sizeof(QUIT_GAME));
    mvwprintw(win, y, x, QUIT_GAME);
    refresh();
}

void keyPadSelector(WINDOW *win, int min_choice, int max_choice, int input, int* counter){
    switch (input){
        case KEY_UP:
            if((*counter)>min_choice){
                (*counter)--;
            }
            break;
        case KEY_DOWN:
            if((*counter)<max_choice){
                (*counter)++;
            }
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
    if((*input) == ASCII_CODE_ENTER){
        switch (counter) {
            case PLAY_GAME_NUMBER:
                //TODO add play function
                break;
            case MODE_GAME_NUMBER:
                //TODO mode play function
                break;
            case QUIT_GAME_NUMBER:
                (*input) = ASCII_CODE_Q;
                break;
        }
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
 * @brief Procedura che gestisce i colori
 * 
 */
void setupColors(){
    start_color();
    //init_color(COLOR_RED, 712,136,136);
    init_color(COLOR_CYAN, 28, 644, 1000); //#07A1FF
    init_color(COLOR_BLUE, 0,464,868);
    init_pair(PAIR_COLOR_LOGO, COLOR_CYAN,COLOR_BLACK); 
    init_pair(PAIR_COLOR_TEXT, COLOR_WHITE,COLOR_BLACK);
}

/**
 * @brief Procedura che data la tipologia di oggetto (testo, logo,...) personalizza il colore
 * 
 * @param win 
 * @param color 
 */
void pickColor(WINDOW* win, int type_color){
    wattron(win, COLOR_PAIR(type_color));
}

int divideByTwo(int coord){
    return coord/2;
}


