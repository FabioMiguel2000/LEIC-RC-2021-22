#include "stateMachine.h"


void updateStateMachine(stateMachine_st *currStateMachine, char *buf, int identity){
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
            if((identity == RECEIVER && buf[0] == C_SET) || (identity == SENDER && buf[0] == C_UA)){
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


void updateStateMachineInformation(stateMachine_st *currStateMachine, char *buf, int identity, unsigned int sequenceNum){
    switch(currStateMachine->currState){
        case START:
            if(buf[0] == FLAG)
                currStateMachine->currState = FLAG_RCV;
            break;
        case FLAG_RCV:
            if(buf[0] == A_CERR){
                currStateMachine->currState = A_RCV;
                currStateMachine->A_field = buf[0];
            }
            else if(buf[0] == FLAG) 
                currStateMachine->currState = FLAG_RCV;
            else 
                currStateMachine->currState = START;
            break;
        case A_RCV:
            if(identity == TRANSMITTER){
                if(buf[0] == C_RR((sequenceNum+1) % 2)){
                    currStateMachine->currState = C_RCV;
                    currStateMachine->C_field = buf[0];
                }
                else if(buf[0] == C_REJ((sequenceNum+1) % 2)){
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
                if(buf[0] == C_I((sequenceNum+1) % 2)){
                    currStateMachine->currState = C_RCV;
                    currStateMachine->C_field = buf[0];
                }
                else if(buf[0] == FLAG){
                    currStateMachine->currState = FLAG_RCV;
                }
                else if(buf[0] != FLAG){
                    currStateMachine->currState = START;
                }
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
            if(identity == TRANSMITTER){
                if(buf[0] == FLAG){
                    currStateMachine->currState = STOP;
                }
                else{
                    currStateMachine->currState = START;
                }
            }
            else{   //identity == RECEIVER
                currStateMachine->currState = INFO;
            }
            break;
        case INFO:
            if(identity == RECEIVER && buf[0] == FLAG){
                currStateMachine->currState = STOP;
            }
        case STOP:
            break;
        default:
            break;

    }
}

// void updateStateMachineInformation(stateMachine_st *currStateMachine, char  buf, int *ch){
//     switch (currStateMachine->currState) {  
//         case START:
//             if(buf == FLAG) 
//                 currStateMachine->currState = FLAG_RCV;
//             break;
//         case FLAG_RCV:
//             if(buf == A_CERR) 
//                 currStateMachine->currState = A_RCV;
//             else 
//                 if(buf == FLAG) 
//                     currStateMachine->currState = FLAG_RCV;
//                 else 
//                     currStateMachine->currState = START;
//             break;
//         case A_RCV:
//             if(buf == CS(0) || buf == CS(1)) { 
//                 *ch = buf;
//                 currStateMachine->currState = C_RCV;
//             }
//             else 
//                 if(buf == FLAG) 
//                     currStateMachine->currState = FLAG_RCV;
//                 else 
//                     currStateMachine->currState = START;
//             break;
//         case C_RCV:
//             if(buf == (A_CERR ^ *ch)) 
// 	              currStateMachine->currState = BCC1_OK;
//             else 
// 	              if(buf == FLAG) 
// 	                  currStateMachine->currState = FLAG_RCV;
//         	      else 
//                     currStateMachine->currState = START;
//             break;
//         case BCC1_OK:
//             if(buf != FLAG && buf!=0) 
//                 currStateMachine->currState = INFO;
//             else 
//                 currStateMachine->currState = START;
//             break;    
//         case INFO:
//             if (buf == FLAG) 
//                 currStateMachine->currState = STOP;
//             break;
//         case STOP: break;    
//         default:
//             currStateMachine->currState = START;
//             break;
//     }
// }
//note: Funciona para o caso do recetor a receber uma trama de informacao, mas ainda falta a verificacao do BCC2 