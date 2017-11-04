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

sosh: sosh.o cmds.o
	gcc -Wall -o sosh sosh.o cmds.o

sosh.o: sosh.c cmds.h
	gcc -Wall -g -c sosh.c

cmds.o: cmds.c
	gcc -Wall -g -c cmds.c

clean:
	rm -f *.o sosh


