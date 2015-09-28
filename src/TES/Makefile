CCFLAGS=-Wall -Wextra -pedantic -g
CC=gcc

EXEC=TES
SOURCES=$(wildcard *.c)
OBJECTS=$(SOURCES:.c=.o)
HEADERS=$(SOURCES:.c=.h)

run: all
	./$(EXEC)

all: $(EXEC)

$(EXEC): $(OBJECTS) utils.o
	$(CC) $(CCFLAGS) $(OBJECTS) utils.o -o $(EXEC)

-include $(OBJECTS:.o=.d)

%.o: %.c
	$(CC) -MMD -c $(CCFLAGS) -o $@ $<

#utils.o: ../utils.c ../utils.h
#	$(CC) $(CCFLAGS) -c -g -o utils.o $<

clean:
	rm -f $(EXEC) $(OBJECTS) #utils.o

cleand:
	rm -f *.d
