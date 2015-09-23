CCFLAGS=-Wall -Wextra -pedantic
CC=gcc

EXECS=TES ECP user
SOURCES=$(wildcard ./src/TES/*.c)
OBJECTS=$(SOURCES:.c=.o)

TES: tes.o tes_utils.o
	$(CC) $(CCFLAGS) -o $@ $<

tes.o: ./src/TES/tes.c ./src/TES/tes.h
	$(CC) $(CCFLAGS) -c -o $@ $<

tes_utils.o: ./src/TES/tes_utils.c ./src/TES/tes_utils.h
	$(CC) $(CCFLAGS) -c -o $@ $<

clean:
	$(RM) $(EXECS) $(OBJECTS)

