#pragma once
#include <stdlib.h>

unsigned char bcc1(unsigned char a, unsigned char c);
unsigned char bcc2(unsigned char *data, int start, int lenght);
unsigned char* build_info_frame(unsigned char *data, int data_size, int sqnum, int* frame_size);
unsigned char* build_ctrl_frame(char address, char command);
