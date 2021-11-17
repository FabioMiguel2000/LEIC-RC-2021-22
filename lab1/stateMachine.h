#ifndef STATEMACHINE_H
#define STATEMACHINE_H

#include "macros.h"


enum stateMachine { START, FLAG_RCV, A_RCV, C_RCV, BCC_OK, STOP};

typedef struct {
    enum stateMachine currState;
    char A_field;
    char C_field;

}stateMachine_st;

void updateStateMachine(stateMachine_st *currStateMachine, char *buf, int identity);
void updateStateMachineI(stateMachine_st *currStateMachine, char *buf, int identity);
#endif