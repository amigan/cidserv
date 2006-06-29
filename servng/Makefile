# makefile
CC=cc
CPPFLAGS=
LIBS=
LDFLAGS=
CFLAGS+=-ansi
SRCS=
OBJS=
BINS=cidd
all: ${BINS}
.depend: $(SRCS)
	mkdep $(CPPFLAGS) $(SRCS)
cidd: $(OBJS)
	$(CC) $(LDFLAGS) -o cidd $(OBJS) $(LIBS)
%.o: %.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $<
clean:
	rm -f *.core *.o *~ .depend cidd
