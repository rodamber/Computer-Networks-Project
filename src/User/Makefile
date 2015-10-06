CCFLAGS=-Wall -Wextra -pedantic -g
CC=gcc

DEBUG_FLAGS=-D 'DEBUG=1'

EXEC=user
SOURCES=$(wildcard *.c) ../utils.c
OBJECTS=$(SOURCES:.c=.o)
HEADERS=$(SOURCES:.c=.h)

all: $(EXEC)

run: all
	./$(EXEC) 78942

$(EXEC): $(OBJECTS)
	$(CC) $(CCFLAGS) $(DEBUG_FLAGS) $(OBJECTS) -o $(EXEC)

-include $(OBJECTS:.o=.d)

%.o: %.c
	$(CC) -MMD -c $(CCFLAGS) -o $@ $<

clean:
	rm -f $(EXEC) $(OBJECTS)

cleand:
	rm -f *.d
