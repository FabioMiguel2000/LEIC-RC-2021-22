/*Non-Canonical Input Processing*/
#include "macros.h"
#include "stateMachine.h"
#include "alarme.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define BAUDRATE B38400
#define MODEMDEVICE "/dev/ttyS1"
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define MAX_BUF 255
#define FALSE 0
#define TRUE 1

volatile int stop=FALSE;
int identity = SENDER;
extern int timeout,timeoutCount;

int main(int argc, char** argv)
{
    int fd, res;
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


    fd = open(argv[1], O_RDWR | O_NOCTTY |O_NONBLOCK);
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
    buf[0] = FLAG;
    buf[1] = A_CERR;
    buf[2] = C_SET;
    buf[3] = BCC(A_CERR, C_SET);      //Will be trated as null character (\0) if = 0x00
    buf[4] = FLAG;


    
    signal(SIGALRM,timeoutHandler);
    
    timeout=0;
    timeoutCount=0;
    stateMachine_st stateMachine;
    stateMachine.currState=START;

    alarm(MAX_TIME);          // 3 seconds timout
    res = write(fd,buf, 5);   //Sends the data to the receiver
    printf("%d bytes written\n", res);

    while (stateMachine.currState!=STOP) {       /* loop for input */
    
      if(timeout){
        if(timeoutCount>=3){
          printf("TIMEOUT, UA not received!\n");
          return 1;
        }
        //printf("alarme conta=%d\n",conta);//sigalarm foi emitido
        res = write(fd,buf, 5); //SENDS DATA TO RECEIVER AGAIN
        timeout=0;
        stateMachine.currState=START;
        alarm(MAX_TIME);
      }
      res = read(fd,buf,1);   /* returns after 1 char have been input */
      buf[res]=0;               /* so we can printf... */

      if(res != -1){
        printf("Sender received:%#x:%d\n", buf[0], res);

      }
      
      updateStateMachine(&stateMachine, buf, identity);
    
    }


  /* 
    O ciclo FOR e as instru��es seguintes devem ser alterados de modo a respeitar 
    o indicado no gui�o 
  */



   
    if ( tcsetattr(fd,TCSANOW,&oldtio) == -1) {
      perror("tcsetattr");
      exit(-1);
    }




    close(fd);
    return 0;
}


