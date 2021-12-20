#ifndef UTILS_H
#define UTILS_H

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include "application.h"

#include "macros.h"

struct timeElapsed timeElapsed;

struct timeElapsed {
    clock_t start;
    clock_t end;
    double timeTaken;
};

int timeout,timeoutCount;

void timeoutHandler();

void disconnectTimeout();

void logError(char *msg);

void logSuccess(char *msg);

void logInfo(char *msg);

void logWarning(char *msg);

void logUsage();

void startTimeElapsed();

void endTimeElapsed();

void logStats();

#endif