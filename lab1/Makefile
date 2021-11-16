CC = gcc
CFLAGS = -Wall
TARGET = application
SRCS = application.c log.c alarme.c stateMachine.c
OBJS = $(SRCS:.c=.o)

application: $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@

%.o: %.c %.h
	$(CC) $(CFLAGS) -MMD -c $< -o $@ 

clean:
	rm *.o *.d $(TARGET)

# sudo socat -d  -d  PTY,link=/dev/ttyS10,mode=777   PTY,link=/dev/ttyS11,mode=777