
#include "alarme.h"


void timeoutHandler()                   // atende alarme
{
	timeout=1;
	timeoutCount++;
	char buf[MAX_SIZE];
	sprintf(buf, "Time-out achieved, count = %i\n", timeoutCount);
	logWarning(buf);

}


