CC=gcc 
CFLAGS=-g -ansi -pedantic -W -Wall -Wundef -Wstrict-prototypes
#CFLAGS=-O2 -ansi -pedantic -W -Wall -Wundef -Wstrict-prototypes 
PROGRAM=rerun

all: rerun

rerun: rerun.o rerun_config.o main.o
#	${CC} ${CFLAGS} -o ${PROGRAM} rerun.o rerun_config.o main.o

main.o: rerun_config.o main.c rerun.h
	${CC} ${CFLAGS} -c -o main.o rerun_config.o main.c

rerun.o: rerun.c rerun.h

rerun_config.o: rerun_config.c
	${CC} ${CFLAGS} -c -o rerun_config.o rerun_config.c


debug:
	${CC} ${CFLAGS} -o ${PROGRAM} rerun.c main.c

clean:
	rm -f ${PROGRAM} *~ *.o

# If doc failed, make sure rerun is in your PATH
doc: 
	help2man -n "Watches for changes to your source files and then executes a command." \
                 -s 1 -s ${PROGRAM}.c -o ${PROGRAM}.1 ${PROGRAM}

valgrind: rerun
	valgrind -v --leak-check=full --show-reachable=yes ./rerun . '*.c' 'echo "C file edited"'

gdb: rerun
	gdb --args ./rerun . '*.c' 'echo "C file edited"'

tests: test_config
	echo "building tests"

test_config: rerun_config.o
	${CC} ${CFLAGS} -o tests/test_config rerun_config.o tests/test_config.c 

