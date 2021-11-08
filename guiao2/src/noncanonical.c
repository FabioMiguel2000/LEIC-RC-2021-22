/*Non-Canonical Input Processing*/
#include "macros.h"


#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>

#define BAUDRATE B38400
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1
#define MAX_BUF 255

volatile int over=FALSE;

enum stateMachine { START, FLAG_RCV, A_RCV, C_RCV, BCC_OK, STOP};

typedef struct {
    enum stateMachine currState;
    char A_field;
    char C_field;

}stateMachine_st;

void updateStateMachine(stateMachine_st *currStateMachine, char *buf){
    switch(currStateMachine->currState){
        case START: 
            if(buf[0] == FLAG){
                currStateMachine->currState = FLAG_RCV;
            }
            break;
        case FLAG_RCV:
            if(buf[0] == A_CERR){
                currStateMachine->currState = A_RCV;
                currStateMachine->A_field = buf[0];
            }
            else if(buf[0] != FLAG){
                currStateMachine->currState = START;
            }
            break;
        case A_RCV:
            if(buf[0] == C_SET){
                currStateMachine->currState = C_RCV;
                currStateMachine->C_field = buf[0];
            }
            else if(buf[0] == FLAG){
                currStateMachine->currState = FLAG_RCV;
            }
            else if(buf[0] != FLAG){
                currStateMachine->currState = START;
            }
            break;
        case C_RCV:
            if(buf[0] == (currStateMachine->A_field ^ currStateMachine->C_field)){ //Check BCC
                currStateMachine->currState = BCC_OK;
            }
            else if(buf[0] == FLAG){
                currStateMachine->currState = FLAG_RCV;

            }
            else if(buf[0] != FLAG){
                currStateMachine->currState = START;
            }
            break;
        case BCC_OK:
            if(buf[0] == FLAG){
                currStateMachine->currState = STOP;
            }
            break;
    }
}



int main(int argc, char** argv)
{
    int fd,c, res;
    struct termios oldtio,newtio;
    char buf[MAX_BUF];

    if ( (argc < 2) || 
  	     ((strcmp("/dev/ttyS10", argv[1])!=0) && 
  	      (strcmp("/dev/ttyS11", argv[1])!=0) )) {
      printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS1\n");
      exit(1);
    }


  /*
    Open serial port device for reading and writing and not as controlling tty
    because we don't want to get killed if linenoise sends CTRL-C.
  */
  
    
    fd = open(argv[1], O_RDWR | O_NOCTTY );
    if (fd <0) {perror(argv[1]); exit(-1); }

    if ( tcgetattr(fd,&oldtio) == -1) { /* save current port settings */
      perror("tcgetattr");
      exit(-1);
    }

    bzero(&newtio, sizeof(newtio));
    newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
    newtio.c_iflag = IGNPAR;
    newtio.c_oflag = 0;

    /* set input mode (non-canonical, no echo,...) */
    newtio.c_lflag = 0;

    newtio.c_cc[VTIME]    = 0;   /* inter-character timer unused */
    newtio.c_cc[VMIN]     = 5;   /* blocking read until 5 chars received */



  /* 
    VTIME e VMIN devem ser alterados de forma a proteger com um temporizador a 
    leitura do(s) pr�ximo(s) caracter(es)
  */



    tcflush(fd, TCIOFLUSH);

    if ( tcsetattr(fd,TCSANOW,&newtio) == -1) {
      perror("tcsetattr");
      exit(-1);
    }

    printf("New termios structure set\n");

    stateMachine_st stateMachine;
    while (stateMachine.currState!=STOP) {       /* loop for input */
      res = read(fd,buf,1);   /* returns after 1 char have been input */
      buf[res]=0;               /* so we can printf... */

      printf(":%#x:%d\n", buf[0], res);
      updateStateMachine(&stateMachine, buf);
    }
    buf[0] = FLAG;
    buf[1] = A_CERR;
    buf[2] = C_UA;
    buf[3] = BCC(A_CERR, C_UA);
    buf[4] = FLAG;
    
    res = write(fd,buf, 5);  //Sends it back to the sender
    printf("%d bytes written\n", res);
  /* 
    O ciclo WHILE deve ser alterado de modo a respeitar o indicado no gui�o 
  */



    tcsetattr(fd,TCSANOW,&oldtio);
    close(fd);
    return 0;
}
