#include "application.h"
#include "msg.h"
#include "data_stuffing.h"

extern int timeout, timeoutCount;
extern int frameISize;

int parseArgs(int argc, char **argv)
{
    if (argc == 3 && strcmp("/dev/ttyS10", argv[2]) == 0)
    {
        applicationLayer.status = TRANSMITTER;
        strcpy(dataFile.filename, argv[1]);
        struct stat fileInfo;
        stat(dataFile.filename, &fileInfo);
        dataFile.filesize = fileInfo.st_size;
        // printf("filename: %s\n", dataFile.filename);
        return 10;
    }
    if (argc == 2 && strcmp("/dev/ttyS11", argv[1]) == 0)
    {
        applicationLayer.status = RECEIVER;
        return 11;
    }
    return -1;
}

int llopen(int portNum, int identity)
{
    int fd;

    //  initiate linkLayer struct
    sprintf(linkLayer.port, "/dev/ttyS%i", portNum);
    // strcpy(&linkLayer.port, tempPort);
    linkLayer.baudRate = BAUDRATE;
    linkLayer.sequenceNumber = identity == TRANSMITTER ? 0 : 1;
    linkLayer.timeout = TIME_OUT_SCS;
    linkLayer.numTransmissions = TIME_OUT_CHANCES;

    struct termios oldtio, newtio;
    fd = open(linkLayer.port, O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (fd < 0)
    {
        logError("Function llopen(), could not open port!\n");
        return -1;
    }

    if (tcgetattr(fd, &oldtio) == -1)
    { /* save current port settings */
        logError("Function llopen(), error on tcgetattr()!\n");
        return -1;
    }

    bzero(&newtio, sizeof(newtio));
    newtio.c_cflag = linkLayer.baudRate | CS8 | CLOCAL | CREAD;
    newtio.c_iflag = IGNPAR;
    newtio.c_oflag = 0;

    /* set input mode (non-canonical, no echo,...) */
    newtio.c_lflag = 0;

    newtio.c_cc[VTIME] = 0; /* inter-character timer unused */
    newtio.c_cc[VMIN] = 5;  /* blocking read until 5 chars received */

    /* VTIME e VMIN devem ser alterados de forma a proteger com um temporizador a 
    leitura do(s) pr�ximo(s) caracter(es)*/
    tcflush(fd, TCIOFLUSH);

    if (tcsetattr(fd, TCSANOW, &newtio) == -1)
    {
        logError("Function llopen(), error on tcgetattr()!\n");
        return -1;
    }

    logSuccess("New termios structure set\n");

    switch (identity)
    {
    case TRANSMITTER:
        if (transmitter_SET(fd) < 0)
        {
            logError("Unable to establish connection with receiver at function llopen()\n");
            return -1;
        }
        break;
    case RECEIVER:
        if (receiver_UA(fd) < 0)
        {
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

void getFrame_SET(char *buf)
{
    buf[0] = FLAG;
    buf[1] = A_CERR;
    buf[2] = C_SET;
    buf[3] = BCC(A_CERR, C_SET); //Will be trated as null character (\0) if = 0x00
    buf[4] = FLAG;
}

void getFrame_UA(char *buf)
{
    buf[0] = FLAG;
    buf[1] = A_CERR;
    buf[2] = C_UA;
    buf[3] = BCC(A_CERR, C_UA);
    buf[4] = FLAG;
}

int receiver_UA(int fd)
{
    int res;
    char buf[MAX_SIZE];
    char msg[MAX_SIZE];
    stateMachine_st stateMachine;
    stateMachine.currState = START;

    logInfo("Receiver waiting to set connection with transmitter...\n");

    while (stateMachine.currState != STOP)
    {                           /* loop for input */
        res = read(fd, buf, 1); /* returns after 1 char have been input */
        buf[res] = 0;           /* so we can printf... */
        if (res != -1)
        {
            sprintf(msg, "Received from Transmitter:%#x:%d\n", buf[0], res);
            logInfo(msg);
            updateStateMachine(&stateMachine, buf, applicationLayer.status);
        }
    }

    getFrame_UA(buf);

    res = write(fd, buf, 5); //Sends UA to the sender

    logSuccess("Connection with transmitter was sucessfully established!\n");

    return 0;
}

int transmitter_SET(int fd)
{
    int res;
    char buf[MAX_SIZE];
    char msg[MAX_SIZE];

    //print before and after to check if changes
    getFrame_SET(buf);

    signal(SIGALRM, timeoutHandler);

    timeout = 0;
    timeoutCount = 0;
    stateMachine_st stateMachine;
    stateMachine.currState = START;

    alarm(TIME_OUT_SCS);     // 3 seconds timout
    res = write(fd, buf, 5); //Sends the data to the receiver
    if (res < 0)
    {
        logError("Unable to send SET to receiver on function transmitter_SET()!\n");
        exit(-1);
    }

    logInfo("SET frame was sent to receiver, trying to establish connection with receiver\n");

    while (stateMachine.currState != STOP)
    { /* loop for input */
        if (timeout)
        {
            if (timeoutCount >= 3)
            {
                logError("TIMEOUT, UA not received!\n");
                exit(-1);
            }
            res = write(fd, buf, 5); //SENDS DATA TO RECEIVER AGAIN
            timeout = 0;
            stateMachine.currState = START;
            alarm(TIME_OUT_SCS);
        }

        res = read(fd, buf, 1); /* returns after 1 char have been input */
        buf[res] = 0;           /* so we can printf... */

        if (res != -1)
        {
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

int sendPacket(int fd)
{

    printf("File size: %ld bytes\n", dataFile.filesize);
    char controlPacket[MAX_SIZE];

    //  Build control packet
    controlPacket[0] = CTRL_PACK_C_START;
    controlPacket[1] = CTRL_PACK_T_SIZE;
    controlPacket[2] = sizeof(dataFile.filesize);
    memcpy(&controlPacket[3], &dataFile.filesize, sizeof(dataFile.filesize));

    controlPacket[3 + sizeof(dataFile.filesize)] = CTRL_PACK_T_NAME;
    controlPacket[4 + sizeof(dataFile.filesize)] = strlen(dataFile.filename);
    memcpy(&controlPacket[5 + sizeof(dataFile.filesize)], &dataFile.filename, strlen(dataFile.filename));

    // printf("strlen of control packet: %li\n", strlen(controlPacket));
    // printf("Sum value of control packet: %li\n", strlen(dataFile.filename) + 5 + sizeof(dataFile.filesize));

    // for (int i = 0; i < (strlen(dataFile.filename) + 5 + sizeof(dataFile.filesize)); i++)
    // {
    //     if (i >= strlen(dataFile.filename) + 5)
    //     {
    //         printf("control packet [%i] = %c\n", i, controlPacket[i]);
    //         continue;
    //     }
    //     printf("control packet [%i] = %#x\n", i, controlPacket[i]);
    // }
    llwrite(fd, controlPacket, strlen(dataFile.filename) + 5 + sizeof(dataFile.filesize));

    int count = 0;

    int dataFileFd = open(dataFile.filename, O_RDONLY);

    if (dataFileFd < -1)
    {
        logError("Unable to open data file!\n");
    }

    char dataPacket[DATA_MAX_SIZE];
    char data[DATA_MAX_SIZE - 4];
    int bytesRead = read(dataFileFd, &data, DATA_MAX_SIZE - 4);

    

    while (bytesRead > 0)
    // while (count < 8)
    {
        // printf("%i, bytes read, count num = %i\n", bytesRead, count);
        dataPacket[0] = CTRL_PACK_C_DATA;
        dataPacket[1] = count % 255;
        // K = l2*256 + l1, where l1 = dataPacket[2] & l2 = dataPacket[3]
        dataPacket[2] = bytesRead / 256;
        dataPacket[3] = bytesRead % 256;

        memcpy(&dataPacket[4], data, bytesRead);
        printf("data sent length=%i=>\n", bytesRead);
        for(int i= 0; i < bytesRead; i++){
            printf("%#x", dataPacket[4+i]);
        }
        printf("\n");
        llwrite(fd, dataPacket, bytesRead + 4);
        printf("sequence number= %i\n", count);
        count++;
        bytesRead = read(dataFileFd, &data, DATA_MAX_SIZE - 4);
    }

    // Send End Control packet
    controlPacket[0] = CTRL_PACK_C_END;
    llwrite(fd, controlPacket, strlen(dataFile.filename) + 5 + sizeof(dataFile.filesize));

    logSuccess("Finish Data transmission!\n");

    return 0;
}

int receivePacket(int fd)
{
    char dataField[WORST_CASE_FRAME_I];
    int bytesRead;
    int sequenceNum = 0;
    int stop = 0;
    off_t totalSizeLoaded = 0;
    while (!stop)
    {
        bytesRead = llread(fd, dataField);
        if (bytesRead < 0)
        {
            continue;
        }

        if (dataField[0] == CTRL_PACK_C_DATA)
        {
            int dataSize;
            if ((sequenceNum % 255) != dataField[1])
            {
                continue;
            }
            dataSize = 256 * dataField[2] + dataField[3];
            if (dataFile.fd > 0)
            { //If already openned

                printf("\nsaving length=%i=>\n", dataSize);
                for(int i =0; i < dataSize; i++){
                    printf("%#x", dataField[4+i]);
                }
                printf("\n");

                write(dataFile.fd, &dataField[4], dataSize);
                totalSizeLoaded += dataSize;
                linkLayer.sequenceNumber = (linkLayer.sequenceNumber + 1) % 2;
                printf("frame size received = %i\n", frameISize);
                printf("sequence num = %i\n", sequenceNum);
                sequenceNum++;

            }
        }
        else if (dataField[0] == CTRL_PACK_C_START)
        {
            int V_fieldSize;
            int bytesProccessed = 1;
            while (bytesProccessed < bytesRead)
            {
                if (dataField[bytesProccessed] == CTRL_PACK_T_SIZE)
                {
                    V_fieldSize = dataField[bytesProccessed + 1];
                    for (int i = 0; i < V_fieldSize; i++)
                    {
                        dataFile.filesize += dataField[bytesProccessed + 2 + i] << 8 * i;
                        // printf("size = %#x\n", dataField[bytesProccessed + 2 + i]);
                    }
                    printf("\ndatafile size is %li\n", dataFile.filesize);

                    bytesProccessed += V_fieldSize + 1;
                }
                else if (dataField[bytesProccessed] == CTRL_PACK_T_NAME)
                {
                    V_fieldSize = dataField[bytesProccessed + 1];
                    for (int i = 0; i < V_fieldSize; i++)
                    {
                        dataFile.filename[i] = dataField[bytesProccessed + 2 + i];

                    }
                    printf("\ndatafile name is %s\n", dataFile.filename);
                    bytesProccessed += V_fieldSize + 1;
                }
                bytesProccessed ++;
            }
            char newFilename[MAX_SIZE+5];
            sprintf(newFilename, "copy_%s", dataFile.filename);
            dataFile.fd = open(newFilename, O_RDWR | O_CREAT, 0777);        //0777 for permission
            if (dataFile.fd < 0)
            {
                logError("Unable to open file to load data!\n");
                return -1;
            }
            linkLayer.sequenceNumber = (linkLayer.sequenceNumber + 1) % 2;
        }
        else if (dataField[0] == CTRL_PACK_C_END)
        {   
            stop = 1;
            char msg[MAX_SIZE];
            if(close(dataFile.fd)<0){
                printf("Error closing the copy file!\n");
            }
            
            if (totalSizeLoaded == dataFile.filesize)
            {
                sprintf(msg, "All %li bytes successfully received!", totalSizeLoaded);
                logSuccess(msg);
            }
            else
            {
                sprintf(msg, "Only %li bytes received, expected %li bytes!", totalSizeLoaded, dataFile.filesize);
                logWarning(msg);
            }
        }
    }
    return 0;
}
//buffer -> Data field stored in the frame I, which has a maximum size of DATA_MAX_SIZE
int llread(int fd, char *buffer)
{
    int res;
    char msg[MAX_SIZE];
    char buf[MAX_SIZE];
    int machineState;
    char response[MAX_SIZE];
    stateMachine_st stateMachine;
    stateMachine.currState = START;


    response[0] = FLAG;
    response[1] = A_CERR;
    response[4] = FLAG;
    while (stateMachine.currState != STOP)
    {                                /* loop for input */
        res = read(fd, buf, 1); /* returns after 1 char have been input */
        buf[res] = 0;                /* so we can printf... */

        if (res != -1)
        {
            // printf(":%#x:", buf[0]);
            // sprintf(msg, "Received from Transmitter:%#x:%d\n", buf[0], res);
            // logInfo(msg);
            machineState = updateStateMachineInformation(&stateMachine, buf, applicationLayer.status, linkLayer.sequenceNumber);
            if (machineState == INCORRECT_C_FIELD || machineState == INCORRECT_BCC1_FIELD)
            {
                response[2] = C_REJ((linkLayer.sequenceNumber + 1) % 2);
                response[3] = BCC(A_CERR, C_REJ((linkLayer.sequenceNumber + 1) % 2));
                res = write(fd, response, 5);
                if (res < 0)
                {
                    logError("Unable to send REJ to transmitter!\n");
                }
            }
        }
    }

    //fazer destuffing ao linklayer.frame
    char destuffedBCC2;
    int stuffedBCC2Size = 2;
    //BCC2 destuffing
    if (linkLayer.frame[frameISize - 3] == ESCAPE && linkLayer.frame[frameISize - 2] == ESC_ESC)
    {
        destuffedBCC2 = ESCAPE;
    }
    else if (linkLayer.frame[frameISize - 3] == ESCAPE && linkLayer.frame[frameISize - 3] == FLAG_ESC)
    {
        destuffedBCC2 = FLAG;
    }
    else
    {
        destuffedBCC2 = linkLayer.frame[frameISize - 2];
        stuffedBCC2Size = 1;
    }
    //Data destuffing and calculate actual BCC2
    int stuffedDataSize = frameISize - 5 - stuffedBCC2Size;
    int destuffedDataSize = 0;
    char expectedBCC2 = 0x00;
    for (int i = 4; i < stuffedDataSize + 4; i++)
    {
        if (linkLayer.frame[i] == ESCAPE)
        {
            if (linkLayer.frame[i + 1] == FLAG_ESC)
            {
                buffer[destuffedDataSize] = FLAG;
            }
            else if (linkLayer.frame[i + 1] == ESC_ESC)
            {
                buffer[destuffedDataSize] = ESCAPE;
            }
            i++;
        }
        else
        {
            buffer[destuffedDataSize] = linkLayer.frame[i];
        }
        expectedBCC2 ^= buffer[destuffedDataSize];
        destuffedDataSize++;
    }
    if (expectedBCC2 != destuffedBCC2)
    {
        logWarning("Incorrect BCC received from receiver");
        response[2] = C_REJ((linkLayer.sequenceNumber + 1) % 2);
        response[3] = BCC(A_CERR, C_REJ((linkLayer.sequenceNumber + 1) % 2));
        res = write(fd, response, 5);
        if (res < 0)
        {
            logError("Unable to send REJ to transmitter!\n");
            return -1;
        }
    }

    // printf("\nInside the llread=>\n");
    // for(int i = 0; i < destuffedDataSize; i++){
    //     printf("%#x", buffer[i]);
    // }
    // printf("\n");
    response[2] = C_RR(linkLayer.sequenceNumber);
    response[3] = BCC(A_CERR, C_RR(linkLayer.sequenceNumber));
    write(fd, response, 5);
    return destuffedDataSize;
}

int llwrite(int fd, char *dataField, int dataLength)
{

    //-----Calculate BCC2, using the data field before stuffing-------
    char BCC2 = dataField[0];
    for (int i = 1; i < dataLength; i++)
    {
        BCC2 ^= dataField[i];
    }
    // printf("BCC2 before stuffing: %#x\n", BCC2);
    //------------------------------------------------------------------

    //----------------Data Field Stuffing--------------------------------
    char *stuffedDataField = (char *)malloc(dataLength); //Data field after stuffing
    int stuffedDataLength = dataLength;                  //Size of dataField after stuffing

    stuffedDataField[0] = dataField[0];
    int stuffed_index = 0;
    for (int i = 0; i < dataLength; i++)
    {
        if (dataField[i] == FLAG)
        {
            stuffedDataLength++;
            stuffedDataField = (char *)realloc(stuffedDataField, stuffedDataLength);
            stuffedDataField[stuffed_index] = ESCAPE;
            stuffedDataField[stuffed_index + 1] = FLAG_ESC;
            stuffed_index += 2;
        }
        else if (dataField[i] == ESCAPE)
        {
            stuffedDataLength++;
            stuffedDataField = (char *)realloc(stuffedDataField, stuffedDataLength);
            stuffedDataField[stuffed_index] = ESCAPE;
            stuffedDataField[stuffed_index + 1] = ESC_ESC;
            stuffed_index += 2;
        }
        else
        {
            stuffedDataField[stuffed_index] = dataField[i];
            stuffed_index += 1;
        }
    }
    // printf("stuffed size: %i\n", stuffedDataLength);
    // for (int i = 0; i < stuffedDataLength; i++)
    // {
    //     printf("data at position [%i] = %#x\n", i, stuffedDataField[i]);
    // }
    //------------------------------------------------------------------

    //---------------BCC2 stuffing----------------------
    char *stuffedBCC2 = (char *)malloc(1);
    int BCC2Length = 1;
    if (BCC2 == FLAG)
    {
        stuffedBCC2 = (char *)realloc(stuffedBCC2, 2);
        stuffedBCC2[0] = ESCAPE;
        stuffedBCC2[1] = FLAG_ESC;
        BCC2Length = 2;
    }
    else if (BCC2 == ESCAPE)
    {
        stuffedBCC2 = (char *)realloc(stuffedBCC2, 2);
        stuffedBCC2[0] = ESCAPE;
        stuffedBCC2[1] = ESC_ESC;
        BCC2Length = 2;
    }
    else
    {
        stuffedBCC2[0] = BCC2;
    }
    // printf("BCC2 after stuffing: %#x\n", stuffedBCC2[0]);
    //--------------------------------------------------

    //----------------Building Frame I---------------------------------
    int frameISize = 5 + BCC2Length + stuffedDataLength; //Size of frame I
    char frameI[frameISize];         //Allocate memory with size of frame I calculated
    // printf("frameISize = %i\n", frameISize);
    frameI[0] = FLAG;
    frameI[1] = A_CERR;
    frameI[2] = C_I(linkLayer.sequenceNumber);
    frameI[3] = BCC(A_CERR, C_I(linkLayer.sequenceNumber));
    memcpy(&frameI[4], stuffedDataField, stuffedDataLength);
    memcpy(&frameI[4 + stuffedDataLength], stuffedBCC2, BCC2Length);
    frameI[4 + stuffedDataLength + BCC2Length] = FLAG;


    //-------------------------------------------------------------------
    signal(SIGALRM, timeoutHandler);

    timeout = 0;
    timeoutCount = 0;
    stateMachine_st stateMachine;
    stateMachine.currState = START;

    char response[MAX_SIZE];
    char msg[MAX_SIZE];
    alarm(TIME_OUT_SCS);                     // set alarm, 3 seconds timout
    // printf("frameI =\n");
    // for (int i = 0; i < frameISize; i++)
    // {
    //     // printf("frameI [%i] = %#x\n", i, frameI[i]);
    //     printf("%#x",frameI[i]);
    // }
    // printf("\n\n");
    int res = write(fd, frameI, frameISize); //Sends the frame I to the receiver
    printf("frameISize = %i\n", frameISize);

    if (res < 0)
    {
        logError("Unable to write frame I to receiver!\n");
        exit(-1);
    }

    //  Waits for response (RR or REJ from receiver), and resends the frame if needed
    while (stateMachine.currState != STOP)
    { /* loop for input */
        if (timeout)
        {
            if (timeoutCount >= 3)
            {
                logError("TIMEOUT, UA not received!\n");
                exit(-1);
            }
            res = write(fd, frameI, 5); //SENDS DATA TO RECEIVER AGAIN
            timeout = 0;
            stateMachine.currState = START;
            alarm(TIME_OUT_SCS);
        }

        res = read(fd, response, 1); /* returns after 1 char have been input */
        response[res] = 0;           /* so we can printf... */

        if (res != -1)
        {
            // sprintf(msg, "Received from Receiver:%#x:%d\n", response[0], res);
            // logInfo(msg);
            updateStateMachineInformation(&stateMachine, response, applicationLayer.status, linkLayer.sequenceNumber);
        }
        //stateMachine.currState = STOP;
    }
    linkLayer.sequenceNumber = (linkLayer.sequenceNumber + 1) %2;
    return frameISize;
}

// void DataStuffing_TEST(){
//     char data[MAX_SIZE];
//     char expected[] = {0x7d, 0x5e, 0x01, 0x02, 0x7d, 0x5d, 0x7d, 0x5d, 0xff};
//     data[0] = FLAG;
//     data[1] = 0x01;
//     data[2] = 0x02;
//     data[3] = ESCAPE;
//     data[4] = ESCAPE;
//     data[5] = 0xFF;
//     char *stuffedData;
//     stuffedData = stuffing(data, sizeof(data));
//     for(int i = 0; i < sizeof(stuffedData); i ++){
//         printf("data at position [%i] = %#x\texpected: %#x\n", i, stuffedData[i], expected[i]);
//     }
// }

int main(int argc, char **argv)
{
    int portNum = parseArgs(argc, argv);

    if (portNum < 0)
    {
        logUsage();
        exit(-1);
    }
    int fd = llopen(portNum, applicationLayer.status);
    if (fd < 0)
    {
        exit(-1);
    }
    switch (applicationLayer.status)
    {
    case TRANSMITTER:
        if (sendPacket(fd) < -1)
        {
            exit(-1);
        }
        break;
    case RECEIVER:
        if(receivePacket(fd) < -1){
            exit(-1);
        }
        break;
    default:
        break;
    }
    // prepareFrameI();

    return 0;
}