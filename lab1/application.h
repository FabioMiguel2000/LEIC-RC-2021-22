#ifndef APPLICATION_H
#define APPLICATION_H

#include 'utils.h'
#include 'macros.h'
#include 'log.h'

struct applicationLayer {
    int fileDescriptor;             /*Descritor correspondente à porta série*/
    int status;                     /*TRANSMITTER | RECEIVER*/
};
void parseArgs(int argc, char** arg);

int main(int argc, char** arg);

#endif
