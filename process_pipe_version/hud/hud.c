#include "hud.h"

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
    wattron(hud_win, COLOR_PAIR(1));
    wcolor_set(hud_win, COLOR_BLACK, NULL);
    //wbkgd(hud_win, COLOR_YELLOW);
    char scelta1[] = "Play the game.";
    printLogo(hud_win, MAX_X);
    wmove(hud_win, 6,0);
    whline(hud_win, ACS_HLINE, getmaxx(hud_win));
    wattron(hud_win, COLOR_PAIR(3));
    //wbkgd(hud_win, COLOR_RED);

    mvwprintw(hud_win, MAX_Y/2, MAX_X/2 - strlen(scelta1), "%s", scelta1);
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

void printLogo (WINDOW* win, int max_x) {

    char logo1[sizeof(LOGO1)] = "", logo2[sizeof(LOGO2)] = "", logo3[sizeof(LOGO3)] = "", logo4[sizeof(LOGO4)] = "";

    strcat(logo1, LOGO1);
    strcat(logo2, LOGO2);
    strcat(logo3, LOGO3);
    strcat(logo4, LOGO4);

    mvwprintw(win, 1, max_x/2 - 30, logo1);
    mvwprintw(win, 2, max_x/2 - 30, logo2);
    mvwprintw(win, 3, max_x/2 - 30, logo3);
    mvwprintw(win, 4, max_x/2 - 30, logo4);

}