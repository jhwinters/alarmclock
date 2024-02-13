all:	clock

LIBS=	../spirit/library/libspirit.a
CFLAGS=-c -I../spirit/include -L$(LIBS) -funsigned-char
OBJS= clock.o settings.o alarms.o
CC=gcc -ansi -pedantic -Wall -D_POSIX_SOURCE -D_DEFAULT_SOURCE
#CC='gcc -ansi -pedantic -D_POSIX_SOURCE -D_DEFAULT_SOURCE -funsigned-char -Wall -Wunused-const-variable=0 -O2'

depend:
	makedepend -Y -- $(CFLAGS) -- *.c

clean:
	-rm -f *.o clock

clock: $(OBJS) $(LIBS)
	gcc -o clock $(OBJS) -L../spirit/library -lspirit -lyaml
# DO NOT DELETE

alarms.o: includes.h ../spirit/include/global.h ../spirit/include/linklist.h
alarms.o: alarms.h settings.h
clock.o: includes.h ../spirit/include/global.h ../spirit/include/linklist.h
clock.o: alarms.h settings.h
settings.o: includes.h ../spirit/include/global.h
settings.o: ../spirit/include/linklist.h alarms.h settings.h
