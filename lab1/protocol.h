#ifndef PROTOCOL_H
#define PROTOCOL_H

#include "macros.h"
#include "utils.h"
#include "stateMachine.h"


struct linkLayer linkLayer;

struct linkLayer {
    char port[20];                  /*Dispositivo /dev/ttySx, x = 0, 1*/
    int baudRate;                   /*Velocidade de transmissão*/
    unsigned int sequenceNumber;    /*Número de sequência da trama: 0, 1*/
    unsigned int timeout;           /*Valor do temporizador: 1 s*/
    unsigned int numTransmissions;  /*Número de tentativas em caso de falha*/
    char frame[WORST_CASE_FRAME_I];           /*Trama*/
};

int llopen(int portNum, int indentity);

int llwrite(int fd, unsigned char *dataField, int dataLength);

int llread(int fd, unsigned char *buffer);

int llclose(int fd,int indentity);
#endif