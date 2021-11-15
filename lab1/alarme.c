
#include "alarme.h"


void timeoutHandler()                   // atende alarme
{
	//printf("alarme # %d\n", conta);
	timeout=1;
	timeoutCount++;
	printf("Time-out achieved, count = %i\n", timeoutCount);

}


