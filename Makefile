CC=gcc
CFLAGS=-g -Wall

OBJECTS=parse.o main.o

all: $(OBJECTS)
	$(CC) -o shave $(OBJECTS)

clean:
	rm -f *.o shave 
