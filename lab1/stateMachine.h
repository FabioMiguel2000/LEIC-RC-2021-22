#ifndef STATEMACHINE_H
#define STATEMACHINE_H

#include "macros.h"
#include "utils.h"
#include "application.h"
#include "protocol.h"


enum stateMachine { START, FLAG_RCV, A_RCV, C_RCV, BCC1_OK, INFO, STOP};

typedef struct {
    enum stateMachine currState;
    unsigned char A_field;
    unsigned char C_field;
    char A_Expected;  //valor esperado no campo A (Command) em função do comando que está a ser recebido
    char C_Expected;	//valor esperado no campo C (Command) em função do comando que está a ser recebido
}stateMachine_st;

void updateStateMachine_CONNECTION(stateMachine_st *currStateMachine, char *buf);
int updateStateMachine_COMMUNICATION(stateMachine_st *currStateMachine, char *buf);
void updateStateMachinellclose(stateMachine_st *currStateMachine, unsigned char *buf, int identity);
#endif
