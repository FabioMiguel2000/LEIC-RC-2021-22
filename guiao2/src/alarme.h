#pragma once

#include <unistd.h>
#include <signal.h>
#include <stdio.h>

int timeout,timeoutCount;
void timeoutHandler();