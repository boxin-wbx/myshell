CC = gcc
CFLAGS = -O3 -Wall -std=gnu99
MAKE = make
RM = rm -f

myshell: main.o builtin.o utility.o
	$(CC) $(CFLAGS) -o myshell main.o builtin.o utility.o

utility.o: utility.c utility.h main.h main.c builtin.c builtin.h
	$(CC) $(CFLAGS) -c utility.c

builtin.o: utility.c utility.h main.h main.c builtin.c builtin.h
	$(CC) $(CFLAGS) -c builtin.c

main.o: utility.c utility.h main.h main.c builtin.c builtin.h
	$(CC) $(CFLAGS) -c main.c

clean:
	$(RM) myshell *.o