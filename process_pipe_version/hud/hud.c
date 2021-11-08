#include "hud.h"

int divideByTwo(int coord){
    return coord/2;
}

void hudMainMenu(WINDOW *win, int max_y, int max_x){
    wattron(win, COLOR_PAIR(1));
    printLogo(win, max_x);
    wmove(win, 6,0);
    wattron(win, COLOR_PAIR(4));
    whline(win, ACS_HLINE, max_x);
}

void mainMenu(WINDOW *win, int max_y, int max_x){
    int getInput, counter = 0;
    do{
        hudMainMenu(win, max_y, max_x);
        printMainMenu(win, max_y, max_x, counter);
        cbreak();
        keypad(win, true);
        getInput = wgetch(win);
        keyPadSelector(win, PLAY_GAME_NUMBER, QUIT_GAME_NUMBER, getInput, &counter);
        refresh();
        
    }while(getInput != 113); // q è il tasto per quittare
}

void printMainMenu(WINDOW *win, int max_y, int max_x, int selection){
    
    mvwprintw(win, divideByTwo(max_y) + SPACE_BETWEEN_CHOICES * PLAY_GAME_NUMBER, divideByTwo(max_x) - divideByTwo(sizeof(MODE_GAME)) - 3, " ");
    mvwprintw(win, divideByTwo(max_y) + SPACE_BETWEEN_CHOICES * MODE_GAME_NUMBER, divideByTwo(max_x) - divideByTwo(sizeof(MODE_GAME)) - 3, " ");
    mvwprintw(win, divideByTwo(max_y) + SPACE_BETWEEN_CHOICES * QUIT_GAME_NUMBER, divideByTwo(max_x) - divideByTwo(sizeof(MODE_GAME)) - 3, " ");

    mvwprintw(win, divideByTwo(max_y) + SPACE_BETWEEN_CHOICES * selection, divideByTwo(max_x) - divideByTwo(sizeof(MODE_GAME)) - 3, ">");

    mvwprintw(win, divideByTwo(max_y), divideByTwo(max_x) - divideByTwo(sizeof(PLAY_GAME)), PLAY_GAME);
    mvwprintw(win, divideByTwo(max_y) + SPACE_BETWEEN_CHOICES, divideByTwo(max_x) - divideByTwo(sizeof(MODE_GAME)), MODE_GAME);
    mvwprintw(win, divideByTwo(max_y) + SPACE_BETWEEN_CHOICES*QUIT_GAME_NUMBER, divideByTwo(max_x) - divideByTwo(sizeof(QUIT_GAME)), QUIT_GAME);
    refresh();
}

void keyPadSelector(WINDOW *win, int min_choice, int max_choice, int input, int* counter){
    cbreak();
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
    nocbreak();
}

/**
 * @brief Stampa il logo del gioco
 * 
 * @param win 
 * @param max_x 
 */
void printLogo (WINDOW* win, int max_x) {
    int halfLenLogo = 30;
    char logo1[sizeof(LOGO2)] = "", logo2[sizeof(LOGO2)] = "", logo3[sizeof(LOGO3)] = "", logo4[sizeof(LOGO4)] = "";

    strcat(logo1, LOGO1);
    strcat(logo2, LOGO2);
    strcat(logo3, LOGO3);
    strcat(logo4, LOGO4);
    
    mvwprintw(win, 1, divideByTwo(max_x) - halfLenLogo, logo1);
    mvwprintw(win, 2, divideByTwo(max_x) - halfLenLogo, logo2);
    mvwprintw(win, 3, divideByTwo(max_x) - halfLenLogo, logo3);
    mvwprintw(win, 4, divideByTwo(max_x) - halfLenLogo, logo4);

}

void setupColors(){
    start_color();
    //init_color(COLOR_RED, 712,136,136);
    init_color(COLOR_BLUE, 0,464,868);
    init_pair(1,COLOR_BLUE,COLOR_BLACK); 
    init_pair(2,COLOR_BLACK,COLOR_WHITE);
    init_pair(3,COLOR_BLACK,COLOR_RED);
    init_pair(4,COLOR_WHITE,COLOR_BLACK);
}

/**
 * @brief Crea la finestra di dialogo
 * 
 */
void mainWindow(){
    WINDOW* hud_win = newwin(0,0,0,0); // dovrebbe essere fullscreen
    const int MAX_Y = getmaxy(hud_win), MAX_X = getmaxx(hud_win);    
    mainMenu(hud_win, MAX_Y, MAX_X);
    nocbreak();
    refresh();
    //wgetch(hud_win);
}

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
    //getch();
    endwin();
}