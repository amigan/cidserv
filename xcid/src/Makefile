# $Amigan: cidserv/xcid/src/Makefile,v 1.2 2004/12/23 23:11:50 dcp1990 Exp $
#gcc -g -I/usr/X11R6/include -L/usr/X11R6/lib -lXt -lX11 -lXaw -o xcid xcid.c network.c wind.c
CFLAGS=-g -Wall
CC=gcc
CPPFLAGS=-I/usr/X11R6/include 
#-DDEBUG
LDFLAGS=-L/usr/X11R6/lib -lXt -lX11 -lXaw
OBJS=xcid.o network.o wind.o
SRCS=xcid.c network.c wind.c
all: xcid
xcid: ${OBJS}
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJS) -o xcid
xcid.o: xcid.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c xcid.c
network.o: network.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c network.c
wind.o: wind.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c wind.c
clean:
	rm -f *.o core.* *.core *~ xcid
