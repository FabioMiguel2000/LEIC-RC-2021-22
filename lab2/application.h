#ifndef APPLICATION_H
#define APPLICATION_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#include "macros.h"

// typedef struct application_params_t{
//     char user[MAX_SIZE];
//     char pass[MAX_SIZE];
//     char host[MAX_SIZE];
//     char url_path[MAX_SIZE];
// }application_params_t;

typedef struct application_params_t{
    char *user;
    char *pass;
    char *host;
    char *url_path;
}application_params_t;

application_params_t application_params;

int parseArgs(int argc, char **argv);
int main(int argc,  char **argv);
void segmentation_fault_handler();



#endif