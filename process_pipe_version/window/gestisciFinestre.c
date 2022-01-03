#include "gestisciFinestre.h"

/**
 * @brief Procedura che si occupa di incrementare o decrementare la variabile counter in base all'input delle frecce direzionali
 * 
 * @param win 
 * @param min_choice valore minimo 
 * @param max_choice valore massimo
 * @param input 
 * @param counter puntatore alla variabile da gestire
 */
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
 * @brief Procedura che si occupa di stampare i menu
 * 
 * @param win 
 * @param max_res 
 * @param selection 
 */
void printMenu (WINDOW* win, Point max_res, int selection, int menu, int row, char vetStringsMenu[row][DIM_MAX_PRINT_MENU]) {
    int i, y, x;
    int yDivided = divideByTwo(max_res.y), xDivided = divideByTwo(max_res.x), indexPrint;
    int longestString = searchLongestString(row, vetStringsMenu);
    int longestStringDivided = divideByTwo(longestString);
    
    for(i = 0; i < row; i++) { //blocco di codice per rimuovere i '>' dal menu
        y = yDivided + SPACE_BETWEEN_CHOICES * i;
        x = xDivided - longestStringDivided - CREATE_OFFSET_SPACES;
        mvwaddch(win, y, x, ' ');
    }

    for(i=0;i<row;i++) {
        y = yDivided + SPACE_BETWEEN_CHOICES * selection;
        x = xDivided - longestStringDivided - CREATE_OFFSET_SPACES;
        mvwaddch(win, y, x, SELECTOR);
        
        y = yDivided + SPACE_BETWEEN_CHOICES*i;
        x = xDivided - divideByTwo(strlen(vetStringsMenu[i]));
        mvwprintw(win, y, x, vetStringsMenu[i]);
    }
    
    refresh();
}

/**
 * @brief Funzione che restituisce la lunghezza della stringa piu' lunga della matrice
 * viene utilizzata per la stampa dei casi del menu
 * 
 * @param vetStrings, contiene le stringhe di stampa
 * @return lunghezza della stringa piu' lunga
 */
int searchLongestString(int row, char vetStrings[row][DIM_MAX_PRINT_MENU]){
    int start = 0;
    int i, length = strlen(vetStrings[start]), temp;
    for(i = start+1; i < row; i++){
        temp = strlen(vetStrings[i]);
        if(length < temp){
            length = temp;
        }
    }
    return length;
}

/**
 * @brief Procedura utilizzata per centrare gli elementi da stampare
 * 
 * @param coord 
 * @return int 
 */
int divideByTwo(int coord){
    return coord/2;
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
    init_pair(PAIR_COLOR_ALLY_SHIP, COLOR_GREEN, COLOR_BLACK);
    init_pair(PAIR_COLOR_BULLET, COLOR_BLUE, COLOR_BLACK);
    init_pair(PAIR_COLOR_ALIEN, COLOR_GREEN, COLOR_BLACK);
    init_pair(PAIR_COLOR_ALIEN_HALF_HEALTH, COLOR_YELLOW, COLOR_BLACK);
    init_pair(PAIR_COLOR_ALIEN_LOW_HEALTH, COLOR_RED, COLOR_BLACK);
    init_pair(PAIR_COLOR_ALIENS_REMAINING, COLOR_RED, COLOR_BLACK);
    init_pair(PAIR_COLOR_BOMB, COLOR_RED, COLOR_BLACK);
    init_pair(PAIR_COLOR_HEART, COLOR_RED, COLOR_BLACK);
    init_pair(PAIR_COLOR_ALIENS_REMAINING, COLOR_RED, COLOR_BLACK);
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

/**
 * @brief Procedura che disattiva il colore selezionato
 * 
 * @param win 
 * @param color 
 */
void turnOffColor(WINDOW* win, int type_color){
    wattroff(win, COLOR_PAIR(type_color));
}
