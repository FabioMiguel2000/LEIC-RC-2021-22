#ifndef STATEMACHINE_H
#define STATEMACHINE_H

#include "macros.h"
#include "log.h"
#include "application.h"


enum stateMachine { START, FLAG_RCV, A_RCV, C_RCV, BCC1_OK, INFO, STOP};

typedef struct {
    enum stateMachine currState;
    unsigned char A_field;
    unsigned char C_field;
    char A_Expected;  //valor esperado no campo A (Command) em função do comando que está a ser recebido
    char C_Expected;	//valor esperado no campo C (Command) em função do comando que está a ser recebido
}stateMachine_st;

void updateStateMachine(stateMachine_st *currStateMachine, char *buf, int identity);
int updateStateMachineInformation(stateMachine_st *currStateMachine, char *buf, int identity, unsigned int sequenceNum);
#endif
