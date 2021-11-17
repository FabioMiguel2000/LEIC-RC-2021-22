#include "data_stuffing.h"
#include "macros.h"
char* stuffing(char data[],int size_data){
    char* stuffed_data=(char *)malloc(size_data*sizeof(char));
    int stuffed_size=size_data*sizeof(char);
    stuffed_data[0]=data[0];
    int stuffed_index=1;
    for(int i=1;i<size_data;i++){
        if(data[i]==FLAG){
            
            stuffed_data=realloc(stuffed_size+sizeof(char));
            stuffed_data[stuffed_index]=ESCAPE;
            stuffed_data[stuffed_index+1]=FLAG_ESC;
            stuffed_index+=2;
        }
        else{
            if(data[i]==ESCAPE){
                stuffed_data=realloc(stuffed_size+sizeof(char));
                stuffed_data[stuffed_index]=ESCAPE;
                stuffed_data[stuffed_index+1]=ESC_ESC;
                stuffed_index+=2;
            }
            else{
                
                stuffed_data[stuffed_index+1]=data[i];
                stuffed_index+=1;
            }
        }
    }
    return stuffed_data;

}