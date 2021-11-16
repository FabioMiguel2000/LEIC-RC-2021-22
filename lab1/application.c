#include "application.h"

applicationLayer applicationLayer;

void parseArgs(int argc, char** arg){

    if ( (argc < 2) || ((strcmp("/dev/ttyS10", argv[1])!=0) && (strcmp("/dev/ttyS11", argv[1])!=0) )) {
      logError("Usage:\t./application <serialPort>\nFor receiver: use port /dev/ttyS10\nFor transmitter: user port /dev/ttyS11");
      exit(-1);
    }

    if(strcmp("/dev/ttyS10", argv[1])==0){
        applicationLayer.status = TRANSMITTER;
        return 1;
    }
    if(strcmp("/dev/ttyS11", argv[1])==0){
        applicationLayer.status = RECEIVER;
        return 1;
    }

}


int main(int argc, char** arg){
    parseArgs(argc, arg);
}