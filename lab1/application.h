#ifndef APPLICATION_H
#define APPLICATION_H


#include "macros.h"
#include "utils.h"
#include "log.h"
#include "alarme.h"
#include "stateMachine.h"

struct applicationLayer {
    int fileDescriptor;             /*Descritor correspondente à porta série*/
    int status;                     /*TRANSMITTER | RECEIVER*/
};


struct linkLayer {
    char port[20];                  /*Dispositivo /dev/ttySx, x = 0, 1*/
    int baudRate;                   /*Velocidade de transmissão*/
    unsigned int sequenceNumber;    /*Número de sequência da trama: 0, 1*/
    unsigned int timeout;           /*Valor do temporizador: 1 s*/
    unsigned int numTransmissions;  /*Número de tentativas em caso de falha*/
    char frame[MAX_SIZE];           /*Trama*/
};

void parseArgs(int argc, char** arg);

int main(int argc, char** arg);

#endif
