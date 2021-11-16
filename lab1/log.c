void logError(char *msg){
    char buf[MAX_BUF]
    sprintf(buf, "ERROR:\t%s\n", msg);
    writef(STDOUT_FILENO, buf, length(buf));
}