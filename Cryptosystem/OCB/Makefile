rm=/bin/rm -f
CC=cc
DEFS=
INCLUDES=-I.
LIBS=

DEFINES= $(INCLUDES) $(DEFS)
CFLAGS= -std=c99 $(DEFINES) -O2 -fomit-frame-pointer -funroll-loops

all: aes128ocb_driver

aes128ocb_driver: aes128ocb_driver.c aes128e.o aes128ocb.o
	$(CC) $(CFLAGS) -o aes128ocb_driver aes128ocb_driver.c aes128e.o aes128ocb.o $(LIBS)

aes128e.o: aes128e.c
	$(CC) $(CFLAGS) -c aes128e.c $(LIBS)

aes128ocb.o: aes128ocb.c
	$(CC) $(CFLAGS) -c aes128ocb.c $(LIBS)

clean:
	$(rm) aes128ocb.o aes128ocb_driver *.o core *~

