#pragma once
#include <stdio.h>
#include <stdlib.h>

unsigned char* stuffing(unsigned char data[],int size_data, int* stuff_size);
unsigned char* unstuffing(unsigned char stuffed[],int stuffed_size, int* data_size);
