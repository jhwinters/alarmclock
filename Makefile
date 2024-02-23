all:	clock

LIBS=	../spirit/library/libspirit.a
CFLAGS=-c -I../spirit/include -L$(LIBS) -funsigned-char
OBJS= clock.o settings.o alarms.o fonts.o image.o utils.o
CC=gcc -ansi -pedantic -Wall -D_POSIX_SOURCE -D_DEFAULT_SOURCE
#CC='gcc -ansi -pedantic -D_POSIX_SOURCE -D_DEFAULT_SOURCE -funsigned-char -Wall -Wunused-const-variable=0 -O2'

depend:
	makedepend -Y -- $(CFLAGS) -- *.c

clean:
	-rm -f *.o clock

clock: $(OBJS) $(LIBS)
	gcc -o clock $(OBJS) -L../spirit/library -lspirit -lyaml -lSDL2 -lSDL2_ttf -l SDL2_image
# DO NOT DELETE

alarms.o: includes.h ../spirit/include/global.h ../spirit/include/logging.h
alarms.o: ../spirit/include/linklist.h utils.h alarms.h fonts.h image.h
alarms.o: settings.h
clock.o: includes.h ../spirit/include/global.h ../spirit/include/logging.h
clock.o: ../spirit/include/linklist.h utils.h alarms.h fonts.h image.h
clock.o: settings.h
fonts.o: includes.h ../spirit/include/global.h ../spirit/include/logging.h
fonts.o: ../spirit/include/linklist.h utils.h alarms.h fonts.h image.h
fonts.o: settings.h
image.o: includes.h ../spirit/include/global.h ../spirit/include/logging.h
image.o: ../spirit/include/linklist.h utils.h alarms.h fonts.h image.h
image.o: settings.h
settings.o: includes.h ../spirit/include/global.h ../spirit/include/logging.h
settings.o: ../spirit/include/linklist.h utils.h alarms.h fonts.h image.h
settings.o: settings.h
utils.o: includes.h ../spirit/include/global.h ../spirit/include/logging.h
utils.o: ../spirit/include/linklist.h utils.h alarms.h fonts.h image.h
utils.o: settings.h
