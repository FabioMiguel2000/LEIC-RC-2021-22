#include "stateMachine.h"

int frameISize;
extern struct linkLayer linkLayer;

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


int updateStateMachineInformation(stateMachine_st *currStateMachine, char *buf, int identity, unsigned int sequenceNum){
    switch(currStateMachine->currState){
        case START:
            if(buf[0] == FLAG){
                currStateMachine->currState = FLAG_RCV;
                if(identity == RECEIVER){
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
                if(identity == RECEIVER){
                    linkLayer.frame[frameISize] = buf[0];
                    frameISize ++;
                }
            }
            else if(buf[0] == FLAG){
                currStateMachine->currState = FLAG_RCV;
                if(identity == RECEIVER){
                    frameISize = 1;
                    linkLayer.frame[0] = buf[0];
                }
            }
            else {
                currStateMachine->currState = START;
                if(identity == RECEIVER){
                    frameISize = 0;
                }
            }
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
                    linkLayer.frame[frameISize] = buf[0];
                    frameISize ++;
                }
                else if(buf[0] != C_I((sequenceNum+1) % 2)){
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
                if(identity == RECEIVER){
                    linkLayer.frame[frameISize] = buf[0];
                    frameISize ++;
                }
            }
            else if(buf[0] != (currStateMachine->A_field ^ currStateMachine->C_field)){
                if(identity == RECEIVER){
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