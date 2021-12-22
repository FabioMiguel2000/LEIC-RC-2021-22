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
/**
 * @brief Disconects timeout .Sets timeout back to 1
 * @return void
 */
void disconnectTimeout();
/**
 * @brief Log Error.
 * 
 * Prints error message in the right format
 * 
 * @param msg    array to put the message 
 * @return void
 */
void logError(char *msg);
/**
 * @brief Log Sucess.
 * 
 * Prints Sucess message in the right format
 * 
 * @param msg    array to put the message 
 * @return void
 */
void logSuccess(char *msg);
/**
 * @brief Log Info.
 * 
 * Prints message with information in the right format
 * 
 * @param msg    array to put the message 
 * @return void
 */
void logInfo(char *msg);
/**
 * @brief Log Warning.
 * 
 * Prints Warning message in the right format
 * 
 * @param msg    array to put the message 
 * @return void
 */
void logWarning(char *msg);
/**
 * @brief Log Usage.
 * 
 * Prints warning message in the right format
 * 
 * @param msg    array to put the message 
 * @return void
 */
void logUsage();

void startTimeElapsed();

void endTimeElapsed();

void logStats();
/**
 * @brief Generate Bcc2 Error must be used before transmitter sending the frame to receiver
 * 
 * @param frame      frame used 
 * @param size       sixe of frame 
 * @param stuffedBCC2Size  size of stuffes bbc2
 * @return void
 */
void generateErrorBCC2(unsigned char *frame, int size, int stuffedBCC2Size);
/**
 * @brief Generates bcc1 errors to test application 
 * 
 * @param frame      frame used 
 * @return void
 */
void generateErrorBCC1(unsigned char *frame);

#endif