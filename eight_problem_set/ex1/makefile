CC = gcc
CFLAGS = -Wall -o

all: pgm.c
	$(CC) $(CFLAGS) pgm pgm.c -lpthread


tests:
	touch tests.txt

	echo "######### Block tests ######## \n" >> tests.txt

	echo "1 thread" >> tests.txt
	./pgm 1 block gator.ascii.pgm result.txt >> tests.txt
	echo "2 threads" >> tests.txt
	./pgm 2 block gator.ascii.pgm result.txt >> tests.txt
	echo "4 threads" >> tests.txt
	./pgm 4 block gator.ascii.pgm result.txt >> tests.txt
	echo "8 threads" >> tests.txt
	./pgm 8 block gator.ascii.pgm result.txt >> tests.txt
	echo "" >> tests.txt

	echo "######### Sign tests ######## \n" >> tests.txt

	echo "1 thread" >> tests.txt
	./pgm 1 sign gator.ascii.pgm result.txt >> tests.txt
	echo "2 threads" >> tests.txt
	./pgm 2 sign gator.ascii.pgm result.txt >> tests.txt
	echo "4 threads" >> tests.txt
	./pgm 4 sign gator.ascii.pgm result.txt >> tests.txt
	echo "8 threads" >> tests.txt
	./pgm 8 sign gator.ascii.pgm result.txt >> tests.txt
	echo "" >> tests.txt

	echo "######### Interleaved tests ######## \n" >> tests.txt

	echo "1 thread" >> tests.txt
	./pgm 1 interleaved gator.ascii.pgm result.txt >> tests.txt
	echo "2 threads" >> tests.txt
	./pgm 2 interleaved gator.ascii.pgm result.txt >> tests.txt
	echo "4 threads" >> tests.txt
	./pgm 4 interleaved gator.ascii.pgm result.txt >> tests.txt
	echo "8 threads" >> tests.txt
	./pgm 8 interleaved gator.ascii.pgm result.txt >> tests.txt
	echo "" >> tests.txt

clean:
	rm -f pgm result.txt tests.txt