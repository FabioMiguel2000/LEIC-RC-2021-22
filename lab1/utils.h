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
/**
 * @brief Handle the Alarm; Checks whether timeout has been reached
 * @return void
 */
void timeoutHandler();

void disconnectTimeout();
/**
 * @brief Log LL activity.
 * 
 * Prints error message in the right format
 * 
 * @param msg    array to put the message 
 * @return void
 */
void logError(char *msg);

void logSuccess(char *msg);

void logInfo(char *msg);

void logWarning(char *msg);

void logUsage();

void startTimeElapsed();

void endTimeElapsed();

void logStats();

void generateErrorBCC2(unsigned char *frame, int size, int stuffedBCC2Size);
/**
 * @brief Generates bcc1 errors to test application 
 * 
 * @param frame      frame used 
 * @return int          Same as printf, or 0 if too verbose
 */
void generateErrorBCC1(unsigned char *frame);

#endif