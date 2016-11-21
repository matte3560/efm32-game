#
# Simple development makefile
#

#
# add more flags here,
# but don't remove the "+="
#
CC = arm-cortexm3-uclinuxeabi-gcc
#CC = gcc
CFLAGS += -O2 -g -Wall #-Wsign-compare -Wfloat-equal -Wformat-security #-Werror
LDFLAGS +=-lm

all: game

game: game.o screen.o input.o
	${CC} ${CFLAGS} -o game game.o screen.o input.o ${LDFLAGS}

game.o: game.c screen.h input.h
	${CC} ${CFLAGS} -c game.c

screen.o: screen.c screen.h
	${CC} ${CFLAGS} -c screen.c
	
input.o: input.c input.h
	${CC} ${CFLAGS} -c input.c
   
clean:
	-rm -f game *.o

install:


.PHONY: all install clean

# end of development makefile
