#include "log.h"

void logError(char *msg){
    char buf[MAX_SIZE];
    sprintf(buf, ">>>ERROR:\t%s\n", msg);
    writef(STDOUT_FILENO, buf, length(buf));
}

void logSuccess(char *msg){
    char buf[MAX_SIZE];
    sprintf(buf, ">>>SUCCESS:\t%s\n", msg);
    writef(STDOUT_FILENO, buf, length(buf));
}

void logInfo(char *msg){
    char buf[MAX_SIZE];
    sprintf(buf, ">>>Info:\t%s\n", msg);
    writef(STDOUT_FILENO, buf, length(buf));
}