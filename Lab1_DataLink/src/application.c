#include "application.h"

int parseArgs(int argc, char **argv)
{   
    if(argc != 2){
        return -1;
    }
    if (strcmp("/dev/ttyS10", argv[1]) == 0)
    {
        return 10;
    }
    if (strcmp("/dev/ttyS11", argv[1]) == 0)
    {
        return 11;
    }
    if (strcmp("/dev/ttyS0", argv[1]) == 0)
    {
        return 0;
    }
    if (strcmp("/dev/ttyS1", argv[1]) == 0)
    {
        return 1;
    }
    return -1;
}

int getIdentity(){
    char buf[MAX_SIZE];
    do{
        logInfo("Choose identity:\n\t\t1.RECEIVER\n\t\t2.TRANSMITTER\n  ");
        fgets(buf, MAX_SIZE, stdin);
        if(buf[strlen(buf)-1] ==  '\n'){  //Removes newline from buffer
            buf[strlen(buf)-1] = '\0';
        }
        if(strcmp("1", buf) == 0 || strcmp("RECEIVER", buf) == 0){
            applicationLayer.status = RECEIVER;
            break;
        }
        if(strcmp("2", buf) == 0 || strcmp("TRANSMITTER", buf) == 0){
            do{
                logInfo("Input name of file to transmit\n");
                fgets(buf, MAX_SIZE, stdin);
                if(buf[strlen(buf)-1] ==  '\n'){  //Removes newline from buffer
                    buf[strlen(buf)-1] = '\0';
                }
                strcpy(dataFile.filename, buf);
                struct stat fileInfo;
                if(stat(dataFile.filename, &fileInfo) == 0){
                    dataFile.filesize = fileInfo.st_size;
                    break;
                }
                logWarning("No file found! Please try again!\n");
            } while (1);
            applicationLayer.status = TRANSMITTER;
            break;
        }
        logWarning("Invalid input! Please try again!\n");
    } while (1);
    return 0;
}

int sendPacket(int fd)
{
    char msg[MAX_SIZE];
    unsigned char controlPacket[MAX_SIZE];

    //  Build control packet
    controlPacket[0] = CTRL_PACK_C_START;
    controlPacket[1] = CTRL_PACK_T_SIZE;
    controlPacket[2] = sizeof(dataFile.filesize);
    memcpy(&controlPacket[3], &dataFile.filesize, sizeof(dataFile.filesize));

    controlPacket[3 + sizeof(dataFile.filesize)] = CTRL_PACK_T_NAME;
    controlPacket[4 + sizeof(dataFile.filesize)] = strlen(dataFile.filename);
    memcpy(&controlPacket[5 + sizeof(dataFile.filesize)], &dataFile.filename, strlen(dataFile.filename));

    if(llwrite(fd, controlPacket, strlen(dataFile.filename) + 5 + sizeof(dataFile.filesize))<0){
        logError("Something went wrong while sending START control packet on llwrite()!\n");
        exit(-1);
    }
    logInfo("START control packet was transmitted!\n");

    int count = 0;

    int dataFileFd = open(dataFile.filename, O_RDONLY);

    if (dataFileFd < -1)
    {
        logError("Unable to open data file!\n");
    }

    unsigned char dataPacket[DATA_MAX_SIZE];
    unsigned char data[DATA_MAX_SIZE - 4];
    

    sprintf(msg, "File Information:\n\t\tFile name: %s\n\t\tFile total size: %ld Bytes\n", dataFile.filename, dataFile.filesize);
    logInfo(msg);
    logInfo("Starting to send file data...\n");

    off_t sizeLeft = dataFile.filesize;
    int bytesRead = read(dataFileFd, &data, DATA_MAX_SIZE - 4);

    while (bytesRead > 0)
    {

        // printf("%i, bytes read, count num = %i\n", bytesRead, count);
        dataPacket[0] = CTRL_PACK_C_DATA;
        dataPacket[1] = count % 255;
        // K = l2*256 + l1, where l1 = dataPacket[2] & l2 = dataPacket[3]
        dataPacket[2] = bytesRead / 256;
        dataPacket[3] = bytesRead % 256;

        memcpy(&dataPacket[4], data, bytesRead);

        if(llwrite(fd, dataPacket, bytesRead + 4) < 0){
            logError("Something went wrong while sending file information on llwrite()!\n");
            exit(-1);
        }
        sizeLeft -= bytesRead;
        sprintf(msg, "Transmission Number = %i\n\t\t> %i Bytes was transmitted on this transmission\n\t\t> %li Bytes left!\n", count, bytesRead, sizeLeft);
        logInfo(msg);
        count++;
        bytesRead = read(dataFileFd, &data, DATA_MAX_SIZE - 4);
    }
    if(sizeLeft == 0){
        sprintf(msg, "All %li Bytes successfully transmitted!\n", dataFile.filesize);
        logSuccess(msg);
    }
    else{
        sprintf(msg, "Only %li bytes transmitted, expected %li bytes!", dataFile.filesize - sizeLeft, dataFile.filesize);
        logWarning(msg);
    }

    // Send End Control packet
    controlPacket[0] = CTRL_PACK_C_END;
    if(llwrite(fd, controlPacket, strlen(dataFile.filename) + 5 + sizeof(dataFile.filesize))<0){
        logError("Something went wrong while sending END control packet on llwrite()!\n");
        exit(-1);
    }

    logInfo("END control packet was transmitted!\n");

    return 0;
}

int receivePacket(int fd)
{
    unsigned char dataField[WORST_CASE_FRAME_I];
    char msg[MAX_SIZE];
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
                sprintf(msg, "Sequence number do not match, received = %i, actual = %i\n", dataField[1], (sequenceNum % 255));
                logWarning(msg);
                continue;
            }
            dataSize = 256 * dataField[2] + dataField[3];
            if (dataFile.fd > 0)
            { //If already openned

                write(dataFile.fd, &dataField[4], dataSize);
                totalSizeLoaded += dataSize;
                sprintf(msg, "Transmission Number = %i\n\t\t> %i Bytes was received on this transmission\n\t\t> %li Bytes total received!\n", sequenceNum, bytesRead, totalSizeLoaded);
                logInfo(msg);

                sequenceNum++;

            }
        }
        else if (dataField[0] == CTRL_PACK_C_START)
        {
            logInfo("START control packet was received!\n");
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
                    }

                    bytesProccessed += V_fieldSize + 1;
                }
                else if (dataField[bytesProccessed] == CTRL_PACK_T_NAME)
                {
                    V_fieldSize = dataField[bytesProccessed + 1];
                    for (int i = 0; i < V_fieldSize; i++)
                    {
                        dataFile.filename[i] = dataField[bytesProccessed + 2 + i];

                    }
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
            sprintf(msg, "File Information:\n\t\tFile name: %s\n\t\tFile total size: %ld Bytes\n", dataFile.filename, dataFile.filesize);
            logInfo(msg);
        }
        else if (dataField[0] == CTRL_PACK_C_END)
        {   
            logInfo("END control packet was received!\n");
            stop = 1;
            if(close(dataFile.fd)<0){
                printf("Error closing the copy file!\n");
            }
            
            if (totalSizeLoaded == dataFile.filesize)
            {
                sprintf(msg, "All %li bytes successfully received!\n", totalSizeLoaded);
                logSuccess(msg);
            }
            else
            {
                sprintf(msg, "Only %li bytes received, expected %li bytes!\n", totalSizeLoaded, dataFile.filesize);
                logWarning(msg);
            }
        }
    }
    return 0;
}

int main(int argc, char **argv)

{
    
    int portNum = parseArgs(argc, argv);

    if (portNum < 0)
    {
        logUsage();
        exit(-1);
    }
    if(getIdentity() < 0){
        logError("Unable to get application identity!\n");
        exit(-1);
    }
    int fd = llopen(portNum, applicationLayer.status);
    if (fd < 0)
    {
        exit(-1);
    }
    startTimeElapsed();
    switch (applicationLayer.status)
    {
    case TRANSMITTER:
        IDENTITY = TRANSMITTER;
        if (sendPacket(fd) < -1)
        {
            exit(-1);
        }
        break;
    case RECEIVER:
        IDENTITY = RECEIVER;
        if(receivePacket(fd) < -1){
            exit(-1);
        }
        break;
    default:
        break;
    }
    endTimeElapsed();
    llclose(fd);
    logStats();
    return 0;
}