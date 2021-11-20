#include "data_stuffing.h"
#include "macros.h"
char* stuffing(int size_data){
    char data[MAX_SIZE];
    data[0] = FLAG;
    data[1] = 0x01;
    data[2] = 0x02;
    data[3] = ESCAPE;
    data[4] = ESCAPE;
    data[5] = 0xFF;
    size_data = 6;

    char* stuffed_data=(char *)malloc(size_data*sizeof(char));
    int stuffed_size=size_data;
    stuffed_data[0]=data[0];
    int stuffed_index=0;
    for(int i=0;i<size_data;i++){
        if(data[i]==FLAG){
            stuffed_size++;
            stuffed_data= (char *) realloc(stuffed_data, stuffed_size);
            stuffed_data[stuffed_index]=ESCAPE;
            stuffed_data[stuffed_index+1]=FLAG_ESC;
            stuffed_index+=2;
        }
        else if(data[i]==ESCAPE){
            stuffed_size++;
            stuffed_data=(char *) realloc(stuffed_data, stuffed_size);
            stuffed_data[stuffed_index]=ESCAPE;
            stuffed_data[stuffed_index+1]=ESC_ESC;
            stuffed_index+=2;
        }
        else{
            stuffed_data[stuffed_index]=data[i];
            stuffed_index+=1;
        }
    }


    for(int i = 0; i < stuffed_size; i ++){
        printf("data at position [%i] = %#x\n", i, stuffed_data[i]);
    }
    return stuffed_data;

}