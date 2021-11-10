#include <unistd.h>
#include <signal.h>
#include <stdio.h>

int flag=0, conta=0;

void atende()                   // atende alarme
{
	//printf("alarme # %d\n", conta);
	flag=1;
	conta++;
   alarm(3);
}


