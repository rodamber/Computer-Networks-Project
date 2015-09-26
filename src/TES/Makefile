CCFLAGS=-Wall -Wextra -pedantic
CC=gcc

EXEC=TES
SOURCES=$(wildcard *.c)
OBJECTS=$(SOURCES:.c=.o)
HEADERS=$(SOURCES:.c=.h)

$(EXEC): $(OBJECTS) utils.o
	$(CC) $(CCFLAGS) -g $(OBJECTS) utils.o -o $(EXEC)

%.o: %.c %.h
	$(CC) $(CCFLAGS) -c -g -o $@ $<

utils.o: ../utils.c ../utils.h
	$(CC) $(CCFLAGS) -c -g -o utils.o $<

clean:
	rm -f $(EXEC) $(OBJECTS) utils.o
