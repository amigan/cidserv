CFLAGS=-g -Wall -DSPEAKER 
# -DDEBUG
all: cidserv bcast hex
cidserv: cidserv.c
	gcc ${CFLAGS} -lutil -o cidserv cidserv.c
bcast: bcast.c
	gcc ${CFLAGS} -o bcast bcast.c
hex: hex.c
	gcc ${CFLAGS} -o hex hex.c
clean:
	rm -f *.o *.core hex bcast cidserv
strip: cidserv bcast hex
	strip cidserv
	strip bcast
	strip hex
