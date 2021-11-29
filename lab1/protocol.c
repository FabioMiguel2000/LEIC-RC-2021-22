#include "protocol.h"

extern int timeout, timeoutCount;
extern int frameISize;

int receiver_UA(int fd)
{
    int res;
    unsigned char buf[MAX_SIZE];
    // char msg[MAX_SIZE];
    stateMachine_st stateMachine;
    stateMachine.currState = START;

    logInfo("Receiver waiting to set connection with transmitter...\n");

    while (stateMachine.currState != STOP)
    {                           /* loop for input */
        res = read(fd, buf, 1); /* returns after 1 char have been input */
        buf[res] = 0;           /* so we can printf... */
        if (res != -1)
        {
            // sprintf(msg, "Received from Transmitter:%#x:%d\n", buf[0], res);
            // logInfo(msg);
            updateStateMachine_CONNECTION(&stateMachine, buf);
        }
    }

    buf[0] = FLAG;
    buf[1] = A_CERR;
    buf[2] = C_UA;
    buf[3] = BCC(A_CERR, C_UA);
    buf[4] = FLAG;

    res = write(fd, buf, 5); //Sends UA to the sender

    logSuccess("Connection with transmitter was sucessfully established!\n");

    return 0;
}

int transmitter_SET(int fd)
{
    int res;
    unsigned char buf[MAX_SIZE];
    // char msg[MAX_SIZE];

    buf[0] = FLAG;
    buf[1] = A_CERR;
    buf[2] = C_SET;
    buf[3] = BCC(A_CERR, C_SET); //Will be trated as null character (\0) if = 0x00
    buf[4] = FLAG;

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

    logInfo("SET sent! Transmitter trying to establish connection with receiver...\n");

    while (stateMachine.currState != STOP)
    { /* loop for input */
        if (timeout)
        {
            if (timeoutCount >= MAX_TRIES)
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

        if (res == 1){
            updateStateMachine_CONNECTION(&stateMachine, buf);
        }
    }
    logSuccess("UA received! Connection with receiver was sucessfully established!\n");

    return 0;
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

int llclose(int fd){
    unsigned char *frame = (unsigned char *)malloc(5);
    unsigned char buf;
    timeout = 0;
    struct termios oldtio;
    stateMachine_st stateMachine;
    int res;
    int numtries = 0;
    signal(SIGALRM, timeoutHandler);
    switch (IDENTITY)
    {
    case TRANSMITTER:
        frame[0] = FLAG;
        frame[1] = A_CERR;
        frame[2] = C_DISC;
        frame[3] = BCC(A_CERR, C_DISC); //envia trama de comando DISC
        frame[4] = FLAG;
        res = write(fd, frame, 5);
        if (res < 0){
            logError("Unable to send DISC to receiver on function llclose()!\n");
            return -1;
        }
        logInfo("DISC frame was sent, trying to disconnect with receiver.\n");
        
        //state machine -aguarda disc do receiver
        stateMachine.currState = START;
        stateMachine.A_Expected = A_CRRE; //aqui
        stateMachine.C_Expected = C_DISC;
        //AGUARDA RESPOSTA TRAMA DE COMANDO DISC COMO RESPOSTA DO RECEIVER
        alarm(TIME_OUT_SCS); // 3 seconds timout
        while (stateMachine.currState != STOP)
        { /* loop for input */
            if (timeout)
            {
                numtries++;
                if (numtries >= MAX_TRIES)
                {
                    logError("TIMEOUT, DISC not received from received!\n");
                    return -1;
                }
                res = write(fd, frame, 5); //SENDS DATA TO RECEIVER AGAIN
                if (res < 0){
                    logError("Could not write DISC to receiver, on function llclose()!\n");
                    return -1;
                }
                timeout = 0;
                stateMachine.currState = START;
                alarm(TIME_OUT_SCS);
            }
            res = read(fd, &buf, 1);
            if(res == 1){
                updateStateMachinellclose(&stateMachine, &buf, IDENTITY);
            }
        }
        logInfo("DISC frame received from receiver.\n");
        //ENVIA TRAMA DE COMANDO UA
        frame[0] = FLAG;
        frame[1] = A_CRRE;
        frame[2] = C_UA;
        frame[3] = BCC(A_CRRE, C_UA);
        frame[4] = FLAG;
        res = write(fd, frame, 5);
        logInfo("UA frame sent to receiver, ready to disconnect.\n");
        if (res < 0)
        {
            logError("Could not respond UA to receiver, on function llclose()!\n");
            return -1;
        }
        break;

    case RECEIVER:
        //STATE MACHINE AGUARDA DISC DO TRANSMITTER
        stateMachine.currState = START;
        stateMachine.A_Expected = A_CERR;
        stateMachine.C_Expected = C_DISC;
        //AGUARDA TRAMA (DISC) COMO COMANDO ENVIADO PELO EMISSOR
        while (stateMachine.currState != STOP)
        {
            res = read(fd, &buf, 1);
            if(res == 1){
                updateStateMachinellclose(&stateMachine, &buf, IDENTITY);
            }
        }
        logInfo("DISC frame received, transmitter asking to disconnect.\n");
        frame[0] = FLAG;
        frame[1] = A_CRRE;
        frame[2] = C_DISC;
        frame[3] = BCC(A_CRRE, C_DISC);
        frame[4] = FLAG;
        res = write(fd, frame, 5); //Envia Comando (DISC) 
        if (res < 0)
        {
            logError("Could not write DISC to transmitter, on function llclose()!\n");
            return -1;
        }
        logInfo("DISC frame was sent, trying to disconnect with transmitter.\n");

        //STATE MACHINE AGUARDA UA DO TRANSMITTER
        stateMachine.currState = START;
        stateMachine.A_Expected = A_CRRE;
        stateMachine.C_Expected = C_UA;
        while (stateMachine.currState != STOP)
        { /* loop for input */
            res = read(fd, &buf, 1); /* returns after 1 char have been input */
            if(res == 1){
                updateStateMachinellclose(&stateMachine, &buf, IDENTITY);
            }
        }
        logInfo("UA frame received, ready to disconnect.\n");
        break;
    default:
        break;
    }

    if (tcsetattr(fd, TCSANOW, &oldtio) == -1)
    {
        perror("tcsetattr");
        return -1;
    }
    close(fd);
    logSuccess("Application successfully terminated!\n");
    return 0;
}

int llwrite(int fd, unsigned char *dataField, int dataLength)
{

    //-----Calculate BCC2, using the data field before stuffing-------
    unsigned char BCC2 = dataField[0];
    for (int i = 1; i < dataLength; i++)
    {
        BCC2 ^= dataField[i];
    }
    // printf("BCC2 before stuffing: %#x\n", BCC2);
    //------------------------------------------------------------------

    //----------------Data Field Stuffing--------------------------------
    unsigned char *stuffedDataField = (unsigned char *)malloc(dataLength); //Data field after stuffing
    int stuffedDataLength = dataLength;                                    //Size of dataField after stuffing

    stuffedDataField[0] = dataField[0];
    int stuffed_index = 0;
    for (int i = 0; i < dataLength; i++)
    {
        if (dataField[i] == FLAG)
        {
            stuffedDataLength++;
            stuffedDataField = (unsigned char *)realloc(stuffedDataField, stuffedDataLength);
            stuffedDataField[stuffed_index] = ESCAPE;
            stuffedDataField[stuffed_index + 1] = FLAG_ESC;
            stuffed_index += 2;
        }
        else if (dataField[i] == ESCAPE)
        {
            stuffedDataLength++;
            stuffedDataField = (unsigned char *)realloc(stuffedDataField, stuffedDataLength);
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
    unsigned char *stuffedBCC2 = (unsigned char *)malloc(1);
    int BCC2Length = 1;
    if (BCC2 == FLAG)
    {
        stuffedBCC2 = (unsigned char *)realloc(stuffedBCC2, 2);
        stuffedBCC2[0] = ESCAPE;
        stuffedBCC2[1] = FLAG_ESC;
        BCC2Length = 2;
    }
    else if (BCC2 == ESCAPE)
    {
        stuffedBCC2 = (unsigned char *)realloc(stuffedBCC2, 2);
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
    unsigned char frameI[frameISize];                    //Allocate memory with size of frame I calculated
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

    unsigned char response[MAX_SIZE];
    // char msg[MAX_SIZE];
    alarm(TIME_OUT_SCS); // set alarm, 3 seconds timout
    // printf("frameI =\n");
    // for (int i = 0; i < frameISize; i++)
    // {
    //     // printf("frameI [%i] = %#x\n", i, frameI[i]);
    //     printf("%#x",frameI[i]);
    // }
    // printf("\n\n");
    int res = write(fd, frameI, frameISize); //Sends the frame I to the receiver
    // printf("frameISize = %i\n", frameISize);
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
            updateStateMachine_COMMUNICATION(&stateMachine, response);
        }
        //stateMachine.currState = STOP;
    }
    linkLayer.sequenceNumber = (linkLayer.sequenceNumber + 1) % 2;
    return frameISize;
}

//buffer -> Data field stored in the frame I, which has a maximum size of DATA_MAX_SIZE
int llread(int fd, unsigned char *buffer)
{
    int res;
    // char msg[MAX_SIZE];
    unsigned char buf[MAX_SIZE];
    int machineState;
    char response[MAX_SIZE];
    stateMachine_st stateMachine;
    stateMachine.currState = START;

    response[0] = FLAG;
    response[1] = A_CERR;
    response[4] = FLAG;
    while (stateMachine.currState != STOP)
    {                           /* loop for input */
        res = read(fd, buf, 1); /* returns after 1 char have been input */
        buf[res] = 0;           /* so we can printf... */

        if (res != -1)
        {
            // printf("received= %#x\n", buf[0]);
            // sprintf(msg, "Received from Transmitter:%#x:%d\n", buf[0], res);
            // logInfo(msg);
            machineState = updateStateMachine_COMMUNICATION(&stateMachine, buf);
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
    unsigned char destuffedBCC2;
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
    unsigned char expectedBCC2 = 0x00;
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
    linkLayer.sequenceNumber = (linkLayer.sequenceNumber + 1) % 2;
    return destuffedDataSize;
}