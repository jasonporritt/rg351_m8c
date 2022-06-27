# Makefile for building gamepad-config

CC	=	gcc

CFLAGS  = -std=c99
INCPATH	= -I/usr/include/SDL2 -D_REENTRANT
LIBS	= -lSDL2


all:
	$(CC) gamepad_config.c -o gamepad_config $(CFLAGS) $(INCPATH) $(LIBS) -DM8C=0

m8c:
	$(CC) gamepad_config.c -o gamepad_config $(CFLAGS) $(INCPATH) $(LIBS) -DM8C=1

patched:
	$(CC) gamepad_config.c -o gamepad_config $(CFLAGS) $(INCPATH) $(LIBS) -DM8C=2

clean:
	-rm -f gamepad_config

install:
	sudo cp gamepad_config /usr/local/bin/
