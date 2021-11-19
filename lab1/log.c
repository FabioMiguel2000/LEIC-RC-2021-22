#include "log.h"

void logError(char *msg){
    char buf[MAX_SIZE];
    sprintf(buf, ">>>ERROR:\t%s\n", msg);
    write(STDOUT_FILENO, buf, strlen(buf));
}

void logSuccess(char *msg){
    char buf[MAX_SIZE];
    sprintf(buf, ">>>SUCCESS:\t%s\n", msg);
    write(STDOUT_FILENO, buf, strlen(buf));
}

void logWarning(char *msg){
    char buf[MAX_SIZE];
    sprintf(buf, ">>>WARNING:\t%s\n", msg);
    write(STDOUT_FILENO, buf, strlen(buf));
}

void logInfo(char *msg){
    char buf[MAX_SIZE];
    sprintf(buf, ">>>Info:\t%s\n", msg);
    write(STDOUT_FILENO, buf, strlen(buf));
}

void logUsage(){
    char buf[MAX_SIZE];
    sprintf(buf, "Usage:\t./application <filename|TRANSMITTER ONLY> <serialPort>\nFor receiver: use port /dev/ttyS10\nFor transmitter: use port /dev/ttyS11\n");
    write(STDOUT_FILENO, buf, strlen(buf));
    
}