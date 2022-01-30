#ifndef APPLICATION_H
#define APPLICATION_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <netdb.h>
#include <netinet/in.h>
#include<arpa/inet.h>
#include <unistd.h>
#include <libgen.h>

#include "macros.h"
#include "log.h"

typedef struct application_params_t{
    char *user;
    char *pass;
    char *host;
    char *url_path;
    char *filename;
}application_params_t;

application_params_t application_params;

int parseArgs(int argc, char **argv);
int main(int argc,  char **argv);
void segmentation_fault_handler();
int getServerResponse(int sockfd, int linesN);
int quit(int sockfd);
int passiveModeRequest(int sockfd);
int download(int sockfd2, int sockfd);
void sendRetr(int sockfd);





#endif