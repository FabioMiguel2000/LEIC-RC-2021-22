#ifndef APPLICATION_H
#define APPLICATION_H


#include "macros.h"
#include "utils.h"
#include "stateMachine.h"
#include "protocol.h"

struct applicationLayer applicationLayer;

struct dataFile dataFile;

struct applicationLayer {
    int fileDescriptor;             /*Descritor correspondente à porta série*/
    int status;                     /*TRANSMITTER | RECEIVER*/
};

struct dataFile {
    char filename[MAX_SIZE];
    off_t filesize;
    int fd;
};


int parseArgs(int argc, char** arg);

int receiver_UA(int fd);

int transmitter_SET(int fd);

int sendPacket(int fd);

int main(int argc, char** arg);

#endif
