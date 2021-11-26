CC = gcc
CFLAGS = -Wall
TARGET = application
OBJS = application.o alarme.o log.o stateMachine.o data_stuffing.o
DEPS = $(OBJS:.o=.h) macros.h utils.h

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

.PHONY: clean

clean:
	rm $(TARGET) $(OBJS) -rf copy_test.png copy_test.txt copy_pinguim.gif

# sudo socat -d  -d  PTY,link=/dev/ttyS10,mode=777   PTY,link=/dev/ttyS11,mode=777