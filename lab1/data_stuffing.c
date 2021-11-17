#include "data_stuffing.h"
#include "macros.h"


unsigned char* stuffing(unsigned char data[],int size_data, int* stuff_size){
    unsigned char* stuffed_data=(unsigned char *)malloc(size_data);
    int stuffed_size=size_data;
    stuffed_data[0]=data[0];
    int stuffed_index=1;
    for(int i=1;i<size_data-1;i++){
        if(data[i]==FLAG){
            stuffed_data=realloc(stuffed_data, stuffed_size+sizeof(char));
            stuffed_data[stuffed_index]=ESCAPE;
            stuffed_data[stuffed_index+1]=FLAG_ESC;
            stuffed_index+=2;
        }
        else{
            if(data[i]==ESCAPE){
                stuffed_data=realloc(stuffed_data, stuffed_size+sizeof(char));
                stuffed_data[stuffed_index]=ESCAPE;
                stuffed_data[stuffed_index+1]=ESC_ESC;
                stuffed_index+=2;
            }
            else{               
                stuffed_data[stuffed_index]=data[i];
                stuffed_index+=1;
            }
        }    
    }
    stuffed_data[stuffed_index]=data[size_data-1];
    stuffed_index+=1;
    *stuff_size=stuffed_index;
    return stuffed_data;
}


unsigned char* unstuffing(unsigned char stuffed[],int stuffed_size, int* data_size){
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
