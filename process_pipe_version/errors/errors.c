#include "errors.h"

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