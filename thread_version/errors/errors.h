#include <stdio.h>
#include <unistd.h>

// Enumerazione che definisce i possibili errori

typedef enum {
    TYPE_EXCEPTION_THREAD_CREATION_FAILURE = 1
} Errors;

// Macro per la stampa degli errori
#define TYPE_EXCEPTION_THREAD_CREATION_FAILURE_TEXT "Errore nella creazione del thread!"
#define GENERIC_EXCEPTION "Errore generico!"

// Dichiarazione delle funzioni e procedure utilizzate per gestire tali errori
void printExceptions(int typeException);