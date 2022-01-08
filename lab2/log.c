#include "log.h"

void logError(char *msg){
    char buf[MAX_SIZE];
    sprintf(buf, "\033[0;31m>>>ERROR:\t%s\n\033[0m", msg);
    write(STDOUT_FILENO, buf, strlen(buf));
}

void logSuccess(char *msg){
    char buf[MAX_SIZE];
    sprintf(buf, "\033[0;32m>>>SUCCESS:\t%s\n\033[0m", msg);
    write(STDOUT_FILENO, buf, strlen(buf));
}

void logWarning(char *msg){
    char buf[MAX_SIZE];
    sprintf(buf, "\033[0;33m>>>WARNING:\t%s\n\033[0m", msg);
    write(STDOUT_FILENO, buf, strlen(buf));
}

void logInfo(char *msg){
    char buf[MAX_SIZE];
    sprintf(buf, ">>>Info:\t%s\n", msg);
    write(STDOUT_FILENO, buf, strlen(buf));
}

void logUsage(){
    char buf[MAX_SIZE];
    sprintf(buf, "Usage: ./download ftp://[<user>:<password>@]<host>/<url-path>\n");
    write(STDOUT_FILENO, buf, strlen(buf));
    
}

void logServer(char *msg){
    char buf[MAX_SIZE];
    sprintf(buf, ">>>Server:\n%s\n", msg);
    write(STDOUT_FILENO, buf, strlen(buf));
}