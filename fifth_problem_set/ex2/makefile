CC = gcc 
CC_flags = -Wall -std=c11

all : main.c
				$(CC) $(CC_flags) main.c -o program
				make tests
				make clean

tests:
				./program test.txt

clean:
				rm -r program



