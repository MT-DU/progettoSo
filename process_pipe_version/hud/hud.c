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
    //wcolor_set(hud_win, COLOR_BLACK, NULL);
    //wbkgd(hud_win, COLOR_YELLOW);
    char scelta1[] = "Play the game.";
    char logo[1000] = "";
    createLogo(logo);
    mvwprintw(hud_win, 0, 0, logo);
    mvwprintw(hud_win, 10, 0, "%d %d", MAX_X, MAX_Y);
    wmove(hud_win, 5,0);
    whline(hud_win, ACS_HLINE, getmaxx(hud_win));
    wattron(hud_win, COLOR_PAIR(3));
    wbkgd(hud_win, COLOR_RED);

    mvwprintw(hud_win, MAX_Y/2, MAX_X/2, "%s", scelta1);
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
    setupColors();
    noecho();
    curs_set(NO_CURSOR);
    mainWindow();
    getch();
    refresh();
    endwin();
}

void createLogo (char sus[300]) {

    strcat(sus,LOGO1);
    strcat(sus,"\n");
    strcat(sus,LOGO2);
    strcat(sus,"\n");
    strcat(sus,LOGO3);
    strcat(sus,"\n");
    strcat(sus,LOGO4);

}