CC = gcc -g
CFLAGS = -c -Wall

all: goodEvil

goodEvil: goodEvil.o
	$(CC) goodEvil.o -o goodEvil -lcrypto

goodEvil.o: goodEvil.c
	$(CC) $(CFLAGS) goodEvil.c -lcrypto

clean:
	rm -f *.o goodEvil
