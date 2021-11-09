#include "stateMachine.h"


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
