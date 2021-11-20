#include "stateMachine.h"


void updateStateMachine(stateMachine_st *currStateMachine, unsigned char buf, int identity){
    switch(currStateMachine->currState){
        case START: 
            if(buf == FLAG){
                currStateMachine->currState = FLAG_RCV;
            }
            break;
        case FLAG_RCV:
            if(buf == currStateMachine->A_Expected){
                currStateMachine->currState = A_RCV;
                currStateMachine->A_field = buf;
            }
            else if(buf != FLAG){
                currStateMachine->currState = START;
            }
            break;
        case A_RCV:
            if(buf == currStateMachine->C_Expected){
                currStateMachine->currState = C_RCV;
                currStateMachine->C_field = buf;
            }
            else if(buf == FLAG){
                    currStateMachine->currState = FLAG_RCV;
                }
                else {
                    currStateMachine->currState = START;
                }
            break;
        case C_RCV:
            if(buf == (currStateMachine->A_field ^ currStateMachine->C_field)){ //Check BCC
                currStateMachine->currState = BCC1_OK;
            }
            else if(buf == FLAG){
                    currStateMachine->currState = FLAG_RCV;

            }
                else {
                    currStateMachine->currState = START;
                }
            break;
        case BCC1_OK:
            if(buf == FLAG){
                currStateMachine->currState = STOP;
            }
            break;
        default:
            break;
    }
}


void updateStateMachineInformation(stateMachine_st *currStateMachine, unsigned char buf, int *ch){
    switch (currStateMachine->currState) {  
        case START:
            if(buf == FLAG) 
                currStateMachine->currState = FLAG_RCV;
            break;
        case FLAG_RCV:
            if(buf == A_CERR) 
                currStateMachine->currState = A_RCV;
            else 
                if(buf == FLAG) 
                    currStateMachine->currState = FLAG_RCV;
                else 
                    currStateMachine->currState = START;
            break;
        case A_RCV:
            if(buf == CS(0) || buf == CS(1)) { 
                *ch = buf;
                currStateMachine->currState = C_RCV;
            }
            else 
                if(buf == FLAG) 
                    currStateMachine->currState = FLAG_RCV;
                else 
                    currStateMachine->currState = START;
            break;
        case C_RCV:
            if(buf == (A_CERR ^ *ch)) 
	              currStateMachine->currState = BCC1_OK;
            else 
	              if(buf == FLAG) 
	                  currStateMachine->currState = FLAG_RCV;
        	      else 
                    currStateMachine->currState = START;
            break;
        case BCC1_OK:
            if(buf != FLAG && buf!=0) 
                currStateMachine->currState = INFO;
            else 
                currStateMachine->currState = START;
            break;    
        case INFO:
            if (buf == FLAG) 
                currStateMachine->currState = STOP;
            break;
        case STOP: break;    
        default:
            currStateMachine->currState = START;
            break;
    }
}
