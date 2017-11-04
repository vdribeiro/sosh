# Makefile
#
# Implicit Rules
#
#    Implicit rules are predefined commands for predefined file types, such
#    as .c, .o and others.
#
#    Implicit rules relieve programmers from having to specify all the
#    commands. Typically, implicit rules are parameterized by macros such as
#    the CC and CFLAG macros.
#
#    Since we are using implicit rules, it is possible to omit the commands
#    out of the rules below


CC = gcc
CFLAGS = -g
all: sosh_s freq 
	
sosh_s: sosh.o cmds.o readline.o
	gcc -Wall -o sosh_s -pthread sosh.o cmds.o readline.o

freq: readline.o freq.o
	gcc -Wall -o  freq -pthread readline.o freq.o

sosh.o: sosh.c cmds.h
	gcc -Wall -g -c -pthread sosh.c

cmds.o: cmds.c cmds.h
	gcc -Wall -g -c -pthread cmds.c

readline.o: readline.c readline.h
	gcc -Wall -g -c -pthread readline.c

freq.o: freq.c freq.h
	gcc -Wall -g -c -pthread freq.c

clean:
	rm -f *.o sosh_s freq


