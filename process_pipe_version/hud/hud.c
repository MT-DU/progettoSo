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
void createWindow(){
    WINDOW* hud_win = newwin(0,0,0,0); // dovrebbe essere fullscreen
    const int MAX_Y = getmaxy(hud_win), MAX_X = getmaxx(hud_win);
    wattron(hud_win, COLOR_PAIR(1));
    //wcolor_set(hud_win, COLOR_BLACK, NULL);
    //wbkgd(hud_win, COLOR_YELLOW);
    char scelta1[] = "Play the game.";
    mvwprintw(hud_win, 0, 5,                                                             
                    "     \t\t  _     ___           _     \n"        
" ___ ___ ___ ___ ___    _| |___|  _|___ ___ _| |___ ___ ___ \n"
"|_ -| . | .'|  _| -_|  | . | -_|  _| -_|   | . | -_|  _|_ -|\n"
"|___|  _|__,|___|___|  |___|___|_| |___|_|_|___|___|_| |___|\n"
    "|_|\n"                                                     );
    wmove(hud_win, 5,0);
    whline(hud_win, ACS_HLINE, getmaxx(hud_win));
    wattron(hud_win, COLOR_PAIR(3));
    wbkgd(hud_win, COLOR_RED);

    mvwprintw(hud_win, (MAX_Y/2) - sizeof(scelta1), (MAX_X/2) - sizeof(scelta1), scelta1);
    refresh();
    wgetch(hud_win);
    // gnome-terminal --geometry=widthxheight+Xposition+Yposition
    // resize -s <rows> <columns>
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
    createWindow();
    getch();
    refresh();
    endwin();
}