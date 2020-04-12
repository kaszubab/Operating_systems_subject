CC = gcc 
CC_flags = -Wall -std=c11

all : main.c 
				$(CC) $(CC_flags) main.c -o program
				make producer_consumer

run:
				./program result.txt tests/test1 tests/test2 tests/test3 tests/test4 tests/test5

clean:
				rm -r producer
				rm -r consumer
				rm -r program
				rm -r result.txt
				rm -r Fifo

producer_consumer: producer.c consumer.c
				$(CC) $(CC_flags) producer.c -o producer
				$(CC) $(CC_flags) consumer.c -o consumer




