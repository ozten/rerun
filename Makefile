CC=gcc 
CFLAGS=-g -ansi -pedantic -W -Wall -Wundef -Wstrict-prototypes
#CFLAGS=-O2 -ansi -pedantic -W -Wall -Wundef -Wstrict-prototypes 
PROGRAM=rerun
all: rerun

rerun: rerun.o main.o
	${CC} ${CFLAGS} -o ${PROGRAM} rerun.o main.o

main.o: main.c rerun.h

rerun.o: rerun.c rerun.h

debug:
	${CC} ${DFLAGS} -o ${PROGRAM} rerun.c main.c

clean:
	rm -f ${PROGRAM} *~

doc: 
	help2man -n "Watches for changes to your source files and then executes a command." \
                 -s 1 -s ${PROGRAM}.c -o ${PROGRAM}.1 ${PROGRAM}

valgrind: rerun
	valgrind -v --leak-check=full --show-reachable=yes ./rerun . '*.c' 'echo "C file edited"'

gdb: rerun
	gdb --args ./rerun . '*.c' 'echo "C file edited"'