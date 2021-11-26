#include "../game/game.h"

/* DEFINIZIONE MACRO */
#define NO_CURSOR 0
#define YES_CURSOR 1
#define ASCII_CODE_q 113
#define ASCII_CODE_Q 81
#define ASCII_CODE_ENTER 10
#define NUMBER_CHOICES 3 //indica il numero di scelte per menu, da modificare nel caso in cui si aumentino i casi dei vari menu

//macro gestione hud
#define Y_HLINE 6
#define MIN_POS 0
#define LOGO1 " ___ ___ ___ ___ ___    _| |___|  _|___ ___ _| |___ ___ ___ "
#define LOGO2 "|_ -| . | .'|  _| -_|  | . | -_|  _| -_|   | . | -_|  _|_ -|"
#define LOGO3 "|___|  _|__,|___|___|  |___|___|_| |___|_|_|___|___|_| |___|"
#define LOGO4 "    |_|                                                     "
#define SIZE_LOGO 61 // lunghezza della stringa della singola macro logo
#define NUM_LOGHI 4 // tiene conto del numero di macro per i logo ci sono

//macro gestione menu principale
#define PLAY_GAME_NUMBER 0
#define MODE_GAME_NUMBER 1
#define QUIT_GAME_NUMBER 2
#define PLAY_GAME "Play the game."
#define MODE_GAME "Select the Game Mode."
#define QUIT_GAME "Quit the game."
#define PRINT_MENU_MAIN 0 
#define LABEL_MAIN_MENU {{PLAY_GAME},{MODE_GAME},{QUIT_GAME}}
#define EXIT_LABEL_HUD "Press q to exit the game"

//macro gestione menu difficolta
#define EASY_MODE_NUMBER 0
#define HARD_MODE_NUMBER 1
#define CUSTOM_MODE_NUMBER 2
#define EASY_MODE "Modalita' Facile"
#define HARD_MODE "Modalita' Difficile"
#define CUSTOM_MODE "Personalizza"
#define PRINT_MENU_GAME_MODE 1 
#define LABEL_GAME_MODE_MENU {{EASY_MODE},{HARD_MODE},{CUSTOM_MODE}}
#define BACK_LABEL_HUD "Press q to go back"


/* PROTOTIPI FUNZIONI / PROCEDURE */
void mainApp();

void mainWindow();
void mainMenu(WINDOW *win, Point max_res);
void hudMainMenu(WINDOW *win, Point max_res, int menu);
void printLogo(WINDOW *win, Point max_res);
void selectOptionMainMenu(WINDOW* win, Point max_res, int* input, int counter);
void gameMode (WINDOW* win, Point max_res);


