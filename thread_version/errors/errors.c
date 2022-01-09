#include "errors.h"

/**
 * @brief Procedura che gestisce i messaggi di errore dell'applicativo
 * 
 * @param typeException Variabile che indica il tipo di errore
 */
void printExceptions(int typeException){
    switch (typeException){
    case TYPE_EXCEPTION_PIPE_CREATION_FAILURE:
        perror(TYPE_EXCEPTION_PIPE_CREATION_FAILURE_TEXT);
        break;
    case TYPE_EXCEPTION_PROCESS_CREATION_FAILURE:
        perror(TYPE_EXCEPTION_PROCESS_CREATION_FAILURE_TEXT);
        break;
    default:
        break;
    }
    _exit(-1);
}