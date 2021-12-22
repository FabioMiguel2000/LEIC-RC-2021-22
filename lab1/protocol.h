#ifndef PROTOCOL_H
#define PROTOCOL_H

#include "macros.h"
#include "utils.h"
#include "stateMachine.h"
#include "application.h"

extern int IDENTITY;

struct linkLayer linkLayer;
/**
 * @brief LinkLayer configuration structure.
 */
struct linkLayer {
    char port[20];                  /*Dispositivo /dev/ttySx, x = 0, 1*/
    int baudRate;                   /*Velocidade de transmissão*/
    unsigned int sequenceNumber;    /*Número de sequência da trama: 0, 1*/
    unsigned int timeout;           /*Valor do temporizador: 1 s*/
    unsigned int numTransmissions;  /*Número de tentativas em caso de falha*/
    char frame[WORST_CASE_FRAME_I];           /*Trama*/
};
/**
 * @brief Opens a data connection with the serial port
 *  
 * @param porta number of the port x in  "/dev/ttySx"
 * @param type TRANSMITTER|RECEIVER
 * @return int idata connection identifier or -1 in case of error
 */
int llopen(int portNum, int identity);

/**
 * @brief Writes to serial port
 *  
 * @param fd Port to where info will be written
 * @param dataField Data that wil be written on port
 * @param dataLength Length of data to be written on port
 * @return number of written characters, negative otherwise.
 */
int llwrite(int fd, unsigned char *dataField, int dataLength);

/**
 * @brief Read Serial Port
 *  
 * @param fd Port to read 
 * @param buffer Data to be read 
 * @return Number of character that have been read, negative if error
 */
int llread(int fd, unsigned char *buffer);

/**
 * @brief Close Serial Port
 *  
 * @param fd Port to close
 * @return int 0 if sucessful,negative otherwise
 */
int llclose(int fd);
#endif