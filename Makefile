CC=		gcc
CFLAGS=		-g -gdwarf-2 -Wall -std=gnu99
LD=		gcc
LDFLAGS=	-L.
TARGETS=	spidey

all:		$(TARGETS)

spidey: forking.o handler.o request.o single.o socket.o spidey.o utils.o
	$(LD) $(LDFLAGS) -o spidey forking.o handler.o request.o single.o socket.o spidey.o utils.o

forking.o: forking.c spidey.h
	$(CC) $(CFLAGS) -o forking.o -c forking.c

handler.o: handler.c spidey.h
	$(CC) $(CFLAGS) -o handler.o -c handler.c

request.o: request.c spidey.h
	$(CC) $(CFLAGS) -o request.o -c request.c

single.o: single.c spidey.h
	$(CC) $(CFLAGS) -o single.o -c single.c

socket.o: socket.c spidey.h
	$(CC) $(CFLAGS) -o socket.o -c socket.c

spidey.o: spidey.c spidey.h
	$(CC) $(CFLAGS) -o spidey.o -c spidey.c

utils.o: utils.c spidey.h
	$(CC) $(CFLAGS) -o utils.o -c utils.c

clean:
	@echo Cleaning...
	@rm -f $(TARGETS) *.o *.log *.input

.PHONY:		all clean
