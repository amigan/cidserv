CFLAGS=-g -Wall
#-DSPEAKER 
# -DDEBUG
all: cidserv bcast hex cnd_mgetty
cidserv: cidserv.c
	cc ${CFLAGS} -lutil -o cidserv cidserv.c
bcast: bcast.c
	cc ${CFLAGS} -o bcast bcast.c
hex: hex.c
	cc ${CFLAGS} -o hex hex.c
cnd_mgetty: cnd_mgetty.c
	cc ${CFLAGS} -o cnd_mgetty cnd_mgetty.c
clean:
	rm -f *.o *.core hex bcast cidserv cnd_mgetty
strip: cidserv bcast hex cnd_mgetty
	strip cidserv
	strip bcast
	strip hex
	cnd_mgetty
