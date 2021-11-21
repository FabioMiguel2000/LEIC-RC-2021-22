#ifndef DATA_STUFFING_H
#define DATA_STUFFING_H

#include "macros.h"
#include "utils.h"

int stuffing(char *data, int size_data, char *stuffed_data);
unsigned char* destuffing(unsigned char stuffed[],int stuffed_size, int* data_size);

#endif
