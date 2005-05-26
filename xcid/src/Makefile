# $Amigan: cidserv/xcid/src/Makefile,v 1.3 2005/05/26 23:40:37 dcp1990 Exp $
#gcc -g -I/usr/X11R6/include -L/usr/X11R6/lib -lXt -lX11 -lXaw -o xcid xcid.c network.c wind.c
CFLAGS=-g -Wall
CC=gcc
CPPFLAGS=-I/usr/X11R6/include -DUSE_XOSD
#-DDEBUG
LDFLAGS=-L/usr/X11R6/lib -lXt -lX11 -lXaw -lxosd -lpthread
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
