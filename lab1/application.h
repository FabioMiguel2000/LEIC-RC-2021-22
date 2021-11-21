#ifndef APPLICATION_H
#define APPLICATION_H


#include "macros.h"
#include "utils.h"
#include "log.h"
#include "alarme.h"
#include "data_stuffing.h"
#include "stateMachine.h"

struct applicationLayer {
    int fileDescriptor;             /*Descritor correspondente à porta série*/
    int status;                     /*TRANSMITTER | RECEIVER*/
};

struct dataFile {
    char filename[MAX_SIZE];
    off_t filesize;
};


struct linkLayer {
    char port[20];                  /*Dispositivo /dev/ttySx, x = 0, 1*/
    int baudRate;                   /*Velocidade de transmissão*/
    unsigned int sequenceNumber;    /*Número de sequência da trama: 0, 1*/
    unsigned int timeout;           /*Valor do temporizador: 1 s*/
    unsigned int numTransmissions;  /*Número de tentativas em caso de falha*/
    char frame[MAX_SIZE];           /*Trama*/
};

int parseArgs(int argc, char** arg);

int llopen(int portNum, int indentity);

void getFrame_UA(char *buf);

void getFrame_SET(char *buf);

int main(int argc, char** arg);

int receiver_UA(int fd);

int transmitter_SET(int fd);

int llwrite(int fd, char *dataField, int dataLength);

// int prepareFrameI();

int sendControlPacket(int fd);

#endif
