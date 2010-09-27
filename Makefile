CC=gcc 
CFLAGS=-ansi -pedantic -W -Wall -Wundef -Wstrict-prototypes -O2
PROGRAM=rerun
all: 
	${CC} ${CFLAGS} -o ${PROGRAM} rerun.c main.c

clean:
	rm -f ${PROGRAM} *~

doc: 
	help2man -n "Watches for changes to your source files and then executes a command." \
                 -s 1 -s ${PROGRAM}.c -o ${PROGRAM}.1 ${PROGRAM}