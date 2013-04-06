rm=/bin/rm -f
CC=cc
DEFS=
INCLUDES=-I.
LIBS=

DEFINES= $(INCLUDES) $(DEFS)
CFLAGS= -std=c99 $(DEFINES) -O2 -fomit-frame-pointer -funroll-loops

all: groestl256_driver

groestl256_driver: groestl256_driver.o groestl256.o
	$(CC) $(CFLAGS) -o groestl256_driver groestl256_driver.c groestl256.o $(LIBS)

groestl256.o: groestl256.c
	$(CC) $(CFLAGS) -c groestl256.c $(LIBS)

clean:
	$(rm) groestl256.o groestl256_driver *.o core *~

