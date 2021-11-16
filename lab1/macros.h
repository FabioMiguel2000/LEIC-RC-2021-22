
#define MAX_SIZE 255
#define TRANSMITTER 0
#define RECEIVER    1


#define BAUDRATE B38400
#define TIME_OUT 3               //tempo maximo de espera para reenvio de trama SET por emissor
#define TIME_OUT_CHANCES 3        //Numero de tentativas de timeout 


/*      FLAG F    */
#define FLAG 0b01111110            //  (0x7E)Flag de inicio ou fim
/*      Campo de endereco A       */
#define A_CERR 0b00000011       // (0x03)Comandos enviados pelo Emissor e Respostas enviadas pelo Receptor
#define A_CRRE 0b00000001       // (0x01)Comandos enviados pelo Receptor e Respostas enviadas pelo Emissor

/*      Campo de controlo C     */

//Tramas de comando 
#define C_SET 0b00000011                    // (0x03)setup
#define C_DISC 0b00001011                   // (0x0B)disconnect
//Tramas de resposta                
#define C_UA 0b00000111                     // (0x07)unnumbered acknowledgment
#define C_RR(r) (0b00000101 | ((r) << 7))   // (0x05 or 0x85)receiver ready / positive ACK
#define C_REJ(r) (0b00000001 | ((r) << 7))  // (0x81 or 0x01)reject / negative ACK

/*      Campo de Proteção (cabeçalho)    */

#define BCC(a,c) (a ^ c)                    // XOR entre Campo A e C


/*              Identity                    */
#define SENDER 0
#define RECEIVER 1
