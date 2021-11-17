#include "msg.h"
#include "macros.h"

unsigned char bcc1(unsigned char a, unsigned char c) {
  return a^c;
}

unsigned char bcc2(unsigned char *data, int start_pos, int lenght){
  unsigned char bcc2 = data[start_pos];
  
  for(int i=start_pos+1; i<lenght; i++){
    bcc2 ^=data[i];
  }
  return bcc2;
}

unsigned char* build_info_frame(unsigned char *data, int data_size, int sqnum, int* frame_size){
  unsigned char*frame = (unsigned char*)malloc(data_size+6);
  frame[0]=FLAG;
  frame[1]=A_SENDER;
  frame[2]=CS(sqnum);
  frame[3]=bcc1(frame[1],frame[2]);
  for(int i=0;i<data_size;i++)
    frame[i+4]=data[i];
  frame[data_size+4]=bcc2(data,0,data_size);
  frame[data_size+5]=FLAG;
  *frame_size=data_size+6;
  return frame;
}

unsigned char* build_ctrl_frame(char field_A, char command){
  unsigned char*frame = (unsigned char*)malloc(5);
  frame[0]=FLAG;
  frame[1]=field_A;
  frame[2]=command;
  frame[3]=bcc1(frame[1],frame[2]);
  frame[4]=field_A;
  return frame;
}

/     build_ctrl_frame(A_SENDER,SET)