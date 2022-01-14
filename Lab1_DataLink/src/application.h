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
 * @return int     Port number that has been passed by user, -1 if error
 */
int parseArgs(int argc, char** arg);

/**
 * @brief Expect UA to arrive from port.
 * 
 * @param fd        file descriptor for serial port connection
 * @return int      On success, 0; -1 otherwise
 */
int receiver_UA(int fd);
/**
 * @brief Transmitter function that handles the file data and sends it to the receiver.
 * 
 * @param fd        file descriptor for serial port connection
 * @return int      O on success, -1 otherwise
 */
int transmitter_SET(int fd);
/**
 * @brief Receiver function that receives and process the file data comming from the transmitter.
 * 
 * @param fd          file descriptor for serial port connection
 * @return int        0 on success, -1 on error
 */
int sendPacket(int fd);

/**
 * @brief Gets the identity that current application (receiver or transmitter), in case of transmitter, will also ask for file name to be send.
 * @return int     return 0 if success
 */
int getIdentity();
/**
 * @brief Main application function.
 * 
 * @param argc          Arguments count
 * @param arg           Argument vector
 * @return int          0 on success, -1 on error
 */
int main(int argc, char** arg);


#endif
