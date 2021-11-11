CC = gcc
CFLAGS = -Wall
TARGETS = receiver sender
SRCS = stateMachine.c


all: $(TARGETS)

sender: writenoncanonical.c $(SRCS) alarme.c
	$(CC) $(CFLAGS) $^ -o $@

receiver: noncanonical.c $(SRCS)
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm $(TARGETS)

# sudo socat -d  -d  PTY,link=/dev/ttyS10,mode=777   PTY,link=/dev/ttyS11,mode=777