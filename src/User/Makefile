CCFLAGS=-Wall -Wextra -pedantic -g
CC=gcc

EXEC=user
SOURCES=$(wildcard *.c) ../utils.c
OBJECTS=$(SOURCES:.c=.o)
HEADERS=$(SOURCES:.c=.h)

all: $(EXEC)

run: all
	./$(EXEC)

$(EXEC): $(OBJECTS)
	$(CC) $(CCFLAGS) $(OBJECTS) -o $(EXEC)

-include $(OBJECTS:.o=.d)

%.o: %.c
	$(CC) -MMD -c $(CCFLAGS) -o $@ $<

clean:
	rm -f $(EXEC) $(OBJECTS)

cleand:
	rm -f *.d
