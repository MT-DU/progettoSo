#include "errors.h"

/**
 * @brief Procedura che gestisce i messaggi di errore dell'applicativo
 * 
 * @param typeException Variabile che indica il tipo di errore
 */
void printExceptions(int typeException){
    switch (typeException){
    case TYPE_EXCEPTION_THREAD_CREATION_FAILURE:
        perror(TYPE_EXCEPTION_THREAD_CREATION_FAILURE_TEXT);
        break;
    default:
        perror(GENERIC_EXCEPTION);
        break;
    }
    _exit(-1);
}