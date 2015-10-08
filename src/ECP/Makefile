CCFLAGS=-Wall -Wextra -pedantic -g -std=c11 -D_GNU_SOURCE
CC=gcc

EXEC=ECP
SOURCES=$(wildcard *.c)
OBJECTS=$(SOURCES:.c=.o)
HEADERS=$(SOURCES:.c=.h)

run: all
	./$(EXEC)

all: $(EXEC)

$(EXEC): $(OBJECTS)
	$(CC) $(CCFLAGS) $(OBJECTS) -o $(EXEC)

-include $(OBJECTS:.o=.d)

%.o: %.c
	$(CC) -MMD -c $(CCFLAGS) -o $@ $<

clean:
	rm -f $(EXEC) $(OBJECTS)

cleand: clean
	rm -f *.d
