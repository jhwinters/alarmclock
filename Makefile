all:	clock

LIBS=	../spirit/library/libspirit.a
CFLAGS=-c -I../spirit/include -L$(LIBS) -funsigned-char
CC=gcc -ansi -pedantic -Wall -D_POSIX_SOURCE -D_DEFAULT_SOURCE
#CC='gcc -ansi -pedantic -D_POSIX_SOURCE -D_DEFAULT_SOURCE -funsigned-char -Wall -Wunused-const-variable=0 -O2'

depend:
	makedepend -Y -- $(CFLAGS) -- *.c

clean:
	-rm -f *.o clock

clock: clock.o settings.o $(LIBS)
	gcc -o clock clock.o settings.o -L../spirit/library -lspirit -lyaml
# DO NOT DELETE

clock.o: ../spirit/include/global.h ../spirit/include/linklist.h settings.h
settings.o: ../spirit/include/global.h settings.h
