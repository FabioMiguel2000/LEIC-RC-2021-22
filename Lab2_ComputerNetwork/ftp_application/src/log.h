#ifndef LOG_H
#define LOG_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "macros.h"

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
 * Prints the correct usage of the application
 * 
 * @return void
 */
void logUsage();


void logServer(char *msg);

#endif