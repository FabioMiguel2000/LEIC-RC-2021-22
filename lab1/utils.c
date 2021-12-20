#include "utils.h"

void logError(char *msg){
    char buf[MAX_SIZE];
    sprintf(buf, "\033[0;31m>>>ERROR:\t%s\n\033[0m", msg);
    write(STDOUT_FILENO, buf, strlen(buf));
}

void logSuccess(char *msg){
    char buf[MAX_SIZE];
    sprintf(buf, "\033[0;32m>>>SUCCESS:\t%s\n\033[0m", msg);
    write(STDOUT_FILENO, buf, strlen(buf));
}

void logWarning(char *msg){
    char buf[MAX_SIZE];
    sprintf(buf, "\033[0;33m>>>WARNING:\t%s\n\033[0m", msg);
    write(STDOUT_FILENO, buf, strlen(buf));
}

void logInfo(char *msg){
    char buf[MAX_SIZE];
    sprintf(buf, ">>>Info:\t%s\n", msg);
    write(STDOUT_FILENO, buf, strlen(buf));
}

void logUsage(){
    char buf[MAX_SIZE];
    sprintf(buf, "Usage:\t./application <SerialPort>\n\tex: ./application /dev/ttyS10\n");
    write(STDOUT_FILENO, buf, strlen(buf));
    
}

void timeoutHandler()                   // atende alarme
{
	timeout=1;
	timeoutCount++;
	char buf[MAX_SIZE];
	sprintf(buf, "Time-out achieved, count = %i\n", timeoutCount);
	logWarning(buf);

}

void disconnectTimeout()                   // atende alarme
{
	timeout=1;
}


void startTimeElapsed(){
    timeElapsed.start = clock();
}

void endTimeElapsed(){
    timeElapsed.end = clock();
    timeElapsed.timeTaken = ((double) (timeElapsed.end - timeElapsed.start)) / CLOCKS_PER_SEC;
}

void logStats(){
    char buf[MAX_SIZE];
    sprintf(buf, "Transmission Time: %fs\nTransmission Rate: %fb/s\n", timeElapsed.timeTaken, dataFile.filesize/timeElapsed.timeTaken);
    write(STDOUT_FILENO, buf, strlen(buf));
}
//Generate Error functions must be used before transmitter sending the frame to receiver
void generateErrorBCC2(unsigned char *frame, int size, int stuffedBCC2Size){
  int errorFlag = (rand() % 100) < ERROR_PROBABILITY_BCC2;
  if (errorFlag){
    int index = (rand() % (size - 1 - stuffedBCC2Size)) + 4;  //Index only in data field range
    frame[index] = frame[index] ^ 0xff;         //Negates a byte
    printf("\nGenerate BCC2 with errors.\nFrame at index: %i = %#x => %#x\n", index, frame[index] ^ 0xff, frame[index]);
  }
}

void generateErrorBCC1(unsigned char *frame){
  int errorFlag = (rand() % 100) < ERROR_PROBABILITY_BCC1;
  if (errorFlag)
  {
    int index = (rand() % 2)+1;   //Index only in header field
    frame[index] = frame[index] ^ 0xff;         //Negates a byte
    printf("\nGenerate BCC1 with errors.\nFrame at index: %i = %#x => %#x\n", index, frame[index] ^ 0xff, frame[index]);

  }
}