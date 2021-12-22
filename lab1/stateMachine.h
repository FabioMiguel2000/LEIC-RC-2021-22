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
    char A_Expected;    //valor esperado no campo A (Command) em função do comando que está a ser recebido
    char C_Expected;	//valor esperado no campo C (Command) em função do comando que está a ser recebido
}stateMachine_st;
/**
 * @brief Update state machine according to received byte to establish connection.
 * 
 * @param currStateMachine   Pointer to state machine
 * @param buf      Byte received, will decide the transition
 * @return int      0 on success, other value otherwise
 */

void updateStateMachine_CONNECTION(stateMachine_st *currStateMachine, unsigned char *buf);
/**
 * @brief Update state machine according to received byte for packet exchange between receiver and transmitter.
 * 
 * @param currStateMachine   Pointer to state machine
 * @param buf      Byte received, will decide the transition
 * @return int      0 on success, other value otherwise
 */
int updateStateMachine_COMMUNICATION(stateMachine_st *currStateMachine, unsigned char *buf);

/**
 * @brief Update state machine according to received byte to be used in llclose in order to close the connection.
 * 
 * @param currStateMachine   Pointer to state machine
 * @param buf      Byte received, will decide the transition
 * @param identity Reference if it is TRANSMITER or RECEIVER
 * @return int      0 on success, other value otherwise
 */
void updateStateMachinell_CLOSE(stateMachine_st *currStateMachine, unsigned char *buf, int identity);
#endif
