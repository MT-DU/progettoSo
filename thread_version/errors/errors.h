#include <stdio.h>
#include <unistd.h>

// Enumerazione che definisce i possibili errori

typedef enum {
    TYPE_EXCEPTION_PIPE_CREATION_FAILURE = 1, TYPE_EXCEPTION_PROCESS_CREATION_FAILURE
} Errors;

// Macro per la stampa degli errori
#define TYPE_EXCEPTION_PIPE_CREATION_FAILURE_TEXT "Errore nella creazione della pipe!"
#define TYPE_EXCEPTION_PROCESS_CREATION_FAILURE_TEXT "Errore nella creazione del processo!"

// Dichiarazione delle funzioni e procedure utilizzate 
void printExceptions(int typeException);