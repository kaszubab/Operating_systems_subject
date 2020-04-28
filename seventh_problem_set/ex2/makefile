
CC = gcc
CFLAGS = -Wall -o

Receivers = 2
Packers = 2
Senders = 2

all: company.c receiver.c packing.c sender.c
	$(CC) $(CFLAGS) company company.c -lrt -lpthread
	$(CC) $(CFLAGS) receiver receiver.c -lrt -lpthread
	$(CC) $(CFLAGS) packing packing.c -lrt -lpthread
	$(CC) $(CFLAGS) sender sender.c -lrt -lpthread

run:
	./company $(Receivers) $(Packers) $(Senders)
clean:
	rm -f receiver packing sender company