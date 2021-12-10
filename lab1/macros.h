#pragma once
#define MAX_SIZE 255
#define TRANSMITTER 0
#define RECEIVER    1
#define DATA_MAX_SIZE 1024
#define WORST_CASE_FRAME_I (DATA_MAX_SIZE*2 + 2 + 5) // Considering every data byte stuffed and BCC2 stuffed + 5 (Flag, field A, field C, BCC1, FLAG)

#define MAX_TIME 3               //  Tempo de espera até reenvio de trama SET pelo Emissor
#define MAX_TRIES 3
#define BAUDRATE B38400
#define TIME_OUT_SCS 3               //tempo maximo de espera para reenvio de trama SET por emissor
#define TIME_OUT_CHANCES 3        //Numero de tentativas de timeout 


/*          Control Packge        */
#define CTRL_PACK_C_DATA    0x01
#define CTRL_PACK_C_START   0x02
#define CTRL_PACK_C_END     0x03

#define CTRL_PACK_T_SIZE    0x00
#define CTRL_PACK_T_NAME    0x01


/*      FLAG F    */
#define FLAG 0b01111110          //  (0x7E) Flag que marca inicio e fim de cada Trama
// octeto A : endereco 
#define A_CERR 0b00000011       // (0x03)Comandos enviados pelo Emissor e Respostas enviadas pelo Receptor
#define A_CRRE 0b00000001       // (0x01)Comandos enviados pelo Receptor e Respostas enviadas pelo Emissor

/*      Campo de controlo C     */

//Tramas de Informacao
#define C_I(s) ((s == 0)? 0x00 : 0x40)    // s = numero de sequencia em tramas I

//Tramas de comando 
#define C_SET 0b00000011                    // (0x03)setup
#define C_DISC 0b00001011                   // (0x0B)disconnect
//Tramas de resposta                
#define C_UA 0b00000111                     // (0x07)unnumbered acknowledgment
#define C_RR(r) ((r == 0)? 0x05 : 0x85)     // (0x05 or 0x85)receiver ready / positive ACK
#define C_REJ(r) ((r == 0)? 0x01 : 0x81)    // (0x81 or 0x01)reject / negative ACK

/*      Campo de Proteção (cabeçalho)    */

#define BCC(a,c) (a ^ c)                    // XOR entre Campo A e C
#define CS(seq) ((seq == 0)? 0x0 : 0x40)


/*              Identity                    */
#define SENDER 0
#define RECEIVER 1


/*      Stuffed     */
#define ESCAPE 0x7d 
#define FLAG_ESC 0x5E
#define ESC_ESC 0x5D

/*      Errors          */
#define INCORRECT_C_FIELD -1
#define INCORRECT_BCC1_FIELD -2
