#ifndef APPLICATION_H
#define APPLICATION_H


#include "macros.h"
#include "utils.h"
#include "stateMachine.h"
#include "protocol.h"


int IDENTITY;

struct applicationLayer applicationLayer;

struct dataFile dataFile;

struct applicationLayer {
    int fileDescriptor;             /*Descritor correspondente à porta série*/
    int status;                     /*TRANSMITTER | RECEIVER*/
};

struct dataFile {
    char filename[100];
    off_t filesize;
    int fd;
};

/**
 * @brief Parses app arguments
 * 
 * @param argc      Argument count
 * @param arg      Argument vector
 * @return int      number of port that has been passed by user, -1 if error
 */
int parseArgs(int argc, char** arg);

/**
 * @brief Expect UA to arrive from port.
 * 
 * @param fd   Port to expect the frame comes from
 * @return int      On success, 0;-1 otherwise
 */
int receiver_UA(int fd);
/**
 * @brief Send SET message (a S-frame) to serial port.
 * 
 * @param fd   Port to send SET message to
 * @return int      O on success, -1 otherwise
 */
int transmitter_SET(int fd);
/**
 * @brief Sends data packet.
 * 
 * @param fd          Piece of data from the file to be sent
 * @return int        0 on success, -1 on error
 */
int sendPacket(int fd);
/**
 * @brief Initializes the application.
 * 
 * @param argc          Arguments count
 * @param arg           Argument vector
 * @return int          0 on success, -1 on error
 */
int main(int argc, char** arg);
/**
 * @brief Gets app arguments
 * @return int     return 0 if success
 */
int getArgs();

#endif
