CC = gcc 
CC_flags = -Wall -std=c11

all : main.c
				$(CC) $(CC_flags) main.c -o program
				make tests
				make clean

tests:
				./program queue
				@echo "\n"

				./program division
				@echo "\n"

	

clean:
				rm -r program



