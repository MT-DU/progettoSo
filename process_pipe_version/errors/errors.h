#include <stdio.h>
#include <unistd.h>

typedef enum {
    TYPE_EXCEPTION_PIPE_CREATION_FAILURE = 1, TYPE_EXCEPTION_PROCESS_CREATION_FAILURE
} Errors;

/* STAMPA PER GESTIRE LE ECCEZIONI */
#define TYPE_EXCEPTION_PIPE_CREATION_FAILURE_TEXT "Errore nella creazione della pipe!"
#define TYPE_EXCEPTION_PROCESS_CREATION_FAILURE_TEXT "Errore nella creazione del processo!"

void printExceptions(int typeException);