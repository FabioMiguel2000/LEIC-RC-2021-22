#include "data_stuffing.h"
#include "macros.h"
int stuffing(char *data, int size_data, char* stuffed_data){


    stuffed_data=(char *)malloc(size_data*sizeof(char));
    int stuffed_size=size_data;
    // printf("stuffed size = %i\n", stuffed_size);
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
    // for(int i = 0; i < stuffed_size; i ++){
    //     printf("data at position [%i] = %#x\n", i, stuffed_data[i]);
    // }

    return stuffed_size;
}

unsigned char* destuffing(unsigned char stuffed[],int stuffed_size, int* data_size){
    unsigned char* data=(unsigned char *)malloc(stuffed_size);
    data[0]=stuffed[0];
    int data_index=1;
    for(int i=1;i<stuffed_size;i++){
      if (stuffed[i]!=ESCAPE){
        data[data_index]=stuffed[i];
        data_index++;  
      }
      else{
        i++;
        data[data_index]=stuffed[i]^0x20;
        data_index++;        
      }
    }
    data=realloc(data, stuffed_size-data_index);
    *data_size=data_index;
    return data;
}
