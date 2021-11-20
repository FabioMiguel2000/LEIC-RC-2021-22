#include "application.h"


struct applicationLayer applicationLayer;
struct linkLayer linkLayer;
struct dataFile dataFile;
extern int timeout,timeoutCount;

int parseArgs(int argc, char** argv){
    if(argc == 3 && strcmp("/dev/ttyS10", argv[2])==0){
        applicationLayer.status = TRANSMITTER;
        strcpy(dataFile.filename, argv[1]);
        struct stat fileInfo;
        stat(dataFile.filename, &fileInfo);
        dataFile.filesize = fileInfo.st_size;
        // printf("filename: %s\n", dataFile.filename);
        return 10;
    }
    if(argc == 2 && strcmp("/dev/ttyS11", argv[1])==0){
        applicationLayer.status = RECEIVER;
        return 11;
    }
    return -1;
}

int llopen(int portNum, int indentity){
    int fd;

    //  initiate linkLayer struct 
    sprintf(linkLayer.port,"/dev/ttyS%i", portNum);
    // strcpy(&linkLayer.port, tempPort);
    linkLayer.baudRate = BAUDRATE;
    linkLayer.sequenceNumber = 0x00;
    linkLayer.timeout = TIME_OUT_SCS;
    linkLayer.numTransmissions = TIME_OUT_CHANCES;

    struct termios oldtio,newtio;
    fd = open(linkLayer.port, O_RDWR | O_NOCTTY| O_NONBLOCK);
    if (fd <0) {
        logError("Function llopen(), could not open port!\n");
        return -1;
    }

    if ( tcgetattr(fd,&oldtio) == -1) { /* save current port settings */
        logError("Function llopen(), error on tcgetattr()!\n");
        return -1;
    }

    bzero(&newtio, sizeof(newtio));
    newtio.c_cflag = linkLayer.baudRate | CS8 | CLOCAL | CREAD;
    newtio.c_iflag = IGNPAR;
    newtio.c_oflag = 0;

    /* set input mode (non-canonical, no echo,...) */
    newtio.c_lflag = 0;

    newtio.c_cc[VTIME]    = 0;   /* inter-character timer unused */
    newtio.c_cc[VMIN]     = 5;   /* blocking read until 5 chars received */
    
    /* VTIME e VMIN devem ser alterados de forma a proteger com um temporizador a 
    leitura do(s) pr�ximo(s) caracter(es)*/
    tcflush(fd, TCIOFLUSH);

    if ( tcsetattr(fd,TCSANOW,&newtio) == -1) {
        logError("Function llopen(), error on tcgetattr()!\n");
        return -1;
    }

    logSuccess("New termios structure set\n");

    switch (indentity)
    {
    case TRANSMITTER:
        if(transmitter_SET(fd)<0){
            logError("Unable to establish connection with receiver at function llopen()\n");
            return -1;
        }
        break;
    case RECEIVER:
        if(receiver_UA(fd)<0){
            logError("Unable to establish connection with transmitter at function llopen()\n");
            return -1;
        }
        break;
    default:
        logError("Unable to establish connection in function llopen(), incorrect identity\n");
        return -1;
        break;
    }
    return fd;
}


// int llwrite(int fd, char *buffer, int length){

// }

void getFrame_SET(char *buf){
    buf[0] = FLAG;
    buf[1] = A_CERR;
    buf[2] = C_SET;
    buf[3] = BCC(A_CERR, C_SET);      //Will be trated as null character (\0) if = 0x00
    buf[4] = FLAG;
}

void getFrame_UA(char *buf){
    buf[0] = FLAG;
    buf[1] = A_CERR;
    buf[2] = C_UA;
    buf[3] = BCC(A_CERR, C_UA);
    buf[4] = FLAG;
}


int receiver_UA(int fd){
    int res;
    char buf[MAX_SIZE];
    char msg[MAX_SIZE];
    stateMachine_st stateMachine;
    stateMachine.currState=START;

    logInfo("Receiver waiting to set connection with transmitter...\n");

    while (stateMachine.currState!=STOP) {       /* loop for input */
        res = read(fd,buf,1);   /* returns after 1 char have been input */
        buf[res]=0;               /* so we can printf... */
        if(res != -1){
            sprintf(msg, "Received from Transmitter:%#x:%d\n", buf[0], res);
            logInfo(msg);
            updateStateMachine(&stateMachine, buf, applicationLayer.status);
        }
    }

    getFrame_UA(buf);

    res = write(fd,buf, 5);  //Sends UA to the sender
    
    logSuccess("Connection with transmitter was sucessfully established!\n");

    return 0;
}

int transmitter_SET(int fd){
    int res;
    char buf[MAX_SIZE];
    char msg[MAX_SIZE];
    
    getFrame_SET(buf);
    
    signal(SIGALRM,timeoutHandler);
    
    timeout=0;
    timeoutCount=0;
    stateMachine_st stateMachine;
    stateMachine.currState=START;

    alarm(TIME_OUT_SCS);          // 3 seconds timout
    res = write(fd,buf, 5);   //Sends the data to the receiver
    if(res < 0){
        logError("Unable to send SET to receiver on function transmitter_SET()!\n");
        exit(-1);
    }

    logInfo("SET frame was sent to receiver, trying to establish connection with receiver\n");

    while (stateMachine.currState!=STOP) {       /* loop for input */
      if(timeout){
        if(timeoutCount>=3){
          logError("TIMEOUT, UA not received!\n");
          exit(-1);
        }
        res = write(fd,buf, 5); //SENDS DATA TO RECEIVER AGAIN
        timeout=0;
        stateMachine.currState=START;
        alarm(TIME_OUT_SCS);
      }


      res = read(fd,buf,1);   /* returns after 1 char have been input */
      buf[res]=0;               /* so we can printf... */

      if(res != -1){
        sprintf(msg, "Received from Receiver:%#x:%d\n", buf[0], res);
        logInfo(msg);
        updateStateMachine(&stateMachine, buf, applicationLayer.status);
      }
      
    }
    logSuccess("Connection with receiver was sucessfully established!\n");

    return 0;
}

// int prepareFrameI(){
//     char filepath[20] = "test.png";
//     struct stat fileStats;
//     int fd;
//     fd = open(filepath, O_RDONLY);
//     if(fd < 0){
//         logError("Could not open file for frame I!\n");
//         exit(-1);
//     }
//     if(stat(filepath, &fileStats) < 0){
//         logError("Could not open file stat for frame I!\n");
//         exit(-1);
//     }
//     char* file_contents = malloc(fileStats.st_size);
//     read(fd, file_contents, fileStats.st_size);

//     printf("read data: %s\n", file_contents);
//     close(fd);
// }

int sendControlPacket(){

    printf("File size: %ld bytes\n", dataFile.filesize);
    char controlPacket[MAX_SIZE];

    //  Build control packet
    controlPacket[0] = CTRL_PACK_C_START;          
    controlPacket[1] = CTRL_PACK_T_SIZE;
    controlPacket[2] = sizeof(dataFile.filesize);
    memcpy(&controlPacket[3], &dataFile.filesize, sizeof(dataFile.filesize));

    controlPacket[3 + sizeof(dataFile.filesize)] = CTRL_PACK_T_NAME;
    controlPacket[4+sizeof(dataFile.filesize)] = strlen(dataFile.filename);
    memcpy(&controlPacket[5+sizeof(dataFile.filesize)], &dataFile.filename, strlen(dataFile.filename));

    printf("strlen of control packet: %li\n", strlen(controlPacket));
    printf("Sum value of control packet: %li\n", strlen(dataFile.filename) + 5 + sizeof(dataFile.filesize));

    // for(int i = 0; i < (strlen(dataFile.filename) + 5 + sizeof(dataFile.filesize)); i++){
    //     if(i >= strlen(dataFile.filename) + 5){
    //         printf("control packet [%i] = %c\n", i, controlPacket[i]);
    //         continue;
    //     }
    //     printf("control packet [%i] = %#x\n", i, controlPacket[i]);
    // }

    return 0;

} 

int llwrite(int fd, char *buffer, int length){
    return 0;
}


void DataStuffing_TEST(){
    char data[MAX_SIZE];
    char expected[] = {0x7d, 0x5e, 0x01, 0x02, 0x7d, 0x5d, 0x7d, 0x5d, 0xff};
    data[0] = FLAG;
    data[1] = 0x01;
    data[2] = 0x02;
    data[3] = ESCAPE;
    data[4] = ESCAPE;
    data[5] = 0xFF;
    char *stuffedData;
    stuffedData = stuffing(data, sizeof(data));
    for(int i = 0; i < sizeof(stuffedData); i ++){
        printf("data at position [%i] = %#x\texpected: %#x\n", i, stuffedData[i], expected[i]);
    }
}

int main(int argc, char** argv){
    // int portNum = parseArgs(argc, argv);
    // if( portNum < 0){
    //     logUsage();
    //     exit(-1);
    // }
    // int fd = llopen(portNum, applicationLayer.status);
    // if(fd < 0){
    //     exit(-1);
    // }
    // switch (applicationLayer.status)
    // {
    // case TRANSMITTER:
    //     if(sendControlPacket() < -1){
    //         exit(-1);
    //     }
    //     break;
    // case RECEIVER:

    //     break;
    // default:
    //     break;
    // }
    // prepareFrameI();
    DataStuffing_TEST();

    return 0;
    
}