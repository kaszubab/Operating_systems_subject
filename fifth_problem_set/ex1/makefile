CC = gcc 
CC_flags = -Wall -std=c11

all : interpreter.c
				$(CC) $(CC_flags) interpreter.c -o program
				make tests
				make clean

tests:
				./program test.txt

clean:
				rm -r program



