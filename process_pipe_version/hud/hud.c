#include "hud.h"

int divideByTwo(int coord){
    return coord/2;
}

//TODO Da rivedere la logica dello switch, ha piu' utilita' per creare il processo del gioco
void printMainMenu(WINDOW *win, int max_y, int max_x, int selector){
    switch(selector){
        case PLAY_GAME_NUMBER:
            mvwprintw(win, divideByTwo(max_y), divideByTwo(max_x) - divideByTwo(sizeof(PLAY_GAME)), PLAY_GAME);
            break;
        case MODE_GAME_NUMBER:
            mvwprintw(win, divideByTwo(max_y) + SPACE_BETWEEN_CHOICES, divideByTwo(max_x) - divideByTwo(sizeof(MODE_GAME)), MODE_GAME);
            break;
        case QUIT_GAME_NUMBER:
            mvwprintw(win, divideByTwo(max_y) + SPACE_BETWEEN_CHOICES*selector, divideByTwo(max_x) - divideByTwo(sizeof(QUIT_GAME)), QUIT_GAME);
            break;
    }
}

int keyPadSelector(WINDOW *win, int min_y, int max_y, int counter){
    cbreak();
    keypad(win, true);
    int key;
    key = wgetch(win);
    switch (key){
        case KEY_UP:
            if(counter>min_y){
                counter--;
            }
            break;
        case KEY_DOWN:
            if(counter<max_y){
                counter++;
            }
    }
    nocbreak();
    return counter;
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
    init_pair(1,COLOR_BLACK,COLOR_YELLOW); 
    init_pair(2,COLOR_BLACK,COLOR_WHITE);
    init_pair(3,COLOR_BLACK,COLOR_RED);
}

/**
 * @brief Crea la finestra di dialogo
 * 
 */
void mainWindow(){
    WINDOW* hud_win = newwin(0,0,0,0); // dovrebbe essere fullscreen
    const int MAX_Y = getmaxy(hud_win), MAX_X = getmaxx(hud_win);
    int i;
    wattron(hud_win, COLOR_PAIR(1));
    printLogo(hud_win, MAX_X);
    wmove(hud_win, 6,0);
    whline(hud_win, ACS_HLINE, getmaxx(hud_win));
    wattron(hud_win, COLOR_PAIR(3));
    //keyPadSelector(hud_win, MAX_Y, );
    for(i = 0; i < 3; i++){
        printMainMenu(hud_win, MAX_Y, MAX_X, i);
    }
    refresh();
    wgetch(hud_win);

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
    getch();
    endwin();
}