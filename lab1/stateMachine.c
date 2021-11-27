#include "stateMachine.h"

int frameISize;
extern struct linkLayer linkLayer;
extern struct applicationLayer applicationLayer;

void updateStateMachine_CONNECTION(stateMachine_st *currStateMachine, char *buf){
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
            if((applicationLayer.status == RECEIVER && buf[0] == C_SET) || (applicationLayer.status == SENDER && buf[0] == C_UA)){
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
                currStateMachine->currState = BCC1_OK;
            }
            else if(buf[0] == FLAG){
                currStateMachine->currState = FLAG_RCV;

            }
            else if(buf[0] != FLAG){
                currStateMachine->currState = START;
            }
            break;
        case BCC1_OK:
            if(buf[0] == FLAG){
                currStateMachine->currState = STOP;
            }
            else if(buf[0] != FLAG){
                currStateMachine->currState = START;
            }
            break;
        default:
            break;
    }
}


int updateStateMachine_COMMUNICATION(stateMachine_st *currStateMachine, char *buf){
    switch(currStateMachine->currState){
        case START:
            if(buf[0] == FLAG){
                currStateMachine->currState = FLAG_RCV;
                if(applicationLayer.status == RECEIVER){
                    frameISize = 0;
                    linkLayer.frame[frameISize] = buf[0];
                    frameISize ++;
                }

            }
            break;
        case FLAG_RCV:
            if(buf[0] == A_CERR){
                currStateMachine->currState = A_RCV;
                currStateMachine->A_field = buf[0];
                if(applicationLayer.status == RECEIVER){
                    linkLayer.frame[frameISize] = buf[0];
                    frameISize ++;
                }
            }
            else if(buf[0] == FLAG){
                currStateMachine->currState = FLAG_RCV;
                if(applicationLayer.status == RECEIVER){
                    frameISize = 1;
                    linkLayer.frame[0] = buf[0];
                }
            }
            else {
                currStateMachine->currState = START;
                if(applicationLayer.status == RECEIVER){
                    frameISize = 0;
                }
            }
            break;
        case A_RCV:
            if(applicationLayer.status == TRANSMITTER){
                if(buf[0] == C_RR((linkLayer.sequenceNumber+1) % 2)){
                    currStateMachine->currState = C_RCV;
                    currStateMachine->C_field = buf[0];
                    
                }
                else if(buf[0] == C_REJ((linkLayer.sequenceNumber+1) % 2)){
                    logWarning("Frame was rejected by receiver!\n");
                    currStateMachine->currState = START;
                }
                else if(buf[0] == FLAG){
                    currStateMachine->currState = FLAG_RCV;
                }
                else if(buf[0] != FLAG){
                    currStateMachine->currState = START;
                }
            }
            else{   //identity == RECEIVER
                if(buf[0] == C_I((linkLayer.sequenceNumber+1) % 2)){
                    currStateMachine->currState = C_RCV;
                    currStateMachine->C_field = buf[0];
                    linkLayer.frame[frameISize] = buf[0];
                    frameISize ++;
                }
                else if(buf[0] != C_I((linkLayer.sequenceNumber+1) % 2)){
                    currStateMachine->currState = START;
                    frameISize = 0;
                    logWarning("Incorrect Control Field received from transmitter!\n ");
                    return INCORRECT_C_FIELD;
                }
                else if(buf[0] == FLAG){
                    currStateMachine->currState = FLAG_RCV;
                    linkLayer.frame[0] = buf[0];
                    frameISize = 1;
                }
                else if(buf[0] != FLAG){
                    currStateMachine->currState = START;
                    frameISize = 0;
                }
            }
            break;
        case C_RCV:
            if(buf[0] == (currStateMachine->A_field ^ currStateMachine->C_field)){ //Check BCC
                currStateMachine->currState = BCC1_OK;
                if(applicationLayer.status == RECEIVER){
                    linkLayer.frame[frameISize] = buf[0];
                    frameISize ++;
                }
            }
            else if(buf[0] != (currStateMachine->A_field ^ currStateMachine->C_field)){
                if(applicationLayer.status == RECEIVER){
                    currStateMachine->currState = START;
                    frameISize = 0;
                    logWarning("Incorrect BCC1 received from transmitter!\n ");
                    return INCORRECT_BCC1_FIELD;
                }
            }
            else if(buf[0] == FLAG){
                currStateMachine->currState = FLAG_RCV;
                linkLayer.frame[0] = buf[0];
                frameISize = 1;
            }
            else if(buf[0] != FLAG){
                currStateMachine->currState = START;
                frameISize = 0;
            }
            break;
        case BCC1_OK:
            if(applicationLayer.status == TRANSMITTER){
                if(buf[0] == FLAG){
                    currStateMachine->currState = STOP;
                }
                else{
                    currStateMachine->currState = START;
                }
            }
            else{   //identity == RECEIVER
                currStateMachine->currState = INFO;
                linkLayer.frame[frameISize] = buf[0];
                frameISize ++;
            }
            break;
        case INFO:  //Can only be reached by receiver
            linkLayer.frame[frameISize] = buf[0];
            frameISize ++;
            if(buf[0] == FLAG){
                currStateMachine->currState = STOP;
            }
        case STOP:
            break;
        default:
            break;

    }
    return 0;
}

void updateStateMachinellclose(stateMachine_st *currStateMachine, unsigned char *buf, int identity){
  
    switch(currStateMachine->currState){
        case START: 
           
            if(*buf== FLAG){
                currStateMachine->currState = FLAG_RCV;
            }
            break;
        case FLAG_RCV:
            if(*buf== currStateMachine->A_Expected){
                currStateMachine->currState = A_RCV;
                currStateMachine->A_field = *buf;
            }
            else if(*buf != FLAG){
                
                currStateMachine->currState = START;
            }
            break;
        case A_RCV:
            if(*buf== currStateMachine->C_Expected){
                currStateMachine->currState = C_RCV;
                currStateMachine->C_field = *buf;
            }
            else if(*buf == FLAG){
                currStateMachine->currState = FLAG_RCV;
            }
            else if(*buf!= FLAG){
                currStateMachine->currState = START;
            }
            break;
        case C_RCV:
            if(*buf== (currStateMachine->A_field ^ currStateMachine->C_field)){ //Check BCC
                currStateMachine->currState = BCC1_OK;
            }
            else if(*buf== FLAG){
                currStateMachine->currState = FLAG_RCV;

            }
            else if(*buf!= FLAG){
                currStateMachine->currState = START;
            }
            break;
        case BCC1_OK:
            if(*buf== FLAG){
                currStateMachine->currState = STOP;
            }
            else if(*buf!= FLAG){
                currStateMachine->currState = START;
            }
            break;
        default:
            break;
    }
}

