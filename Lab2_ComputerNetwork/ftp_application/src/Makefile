CC = gcc
CFLAGS = -Wall
TARGET = download
OBJS = application.o log.o
DEPS = $(OBJS:.o=.h) macros.h

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

.PHONY: clean

clean:
	rm $(TARGET) $(OBJS)

#	These commands should work

#./download ftp://rcom:rcom@ftp.up.pt/pub/kodi/timestamp.txt
#./download ftp://ftp.up.pt/pub/kodi/screenshots/kodi-addons.jpg
#./download ftp://ftp.up.pt/pub/kodi/demo-files/BBB/bbb_sunflower_1080p_30fps_normal.mp4



