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

clean:
	rm -f $(EXEC) $(OBJECTS)

cleand:
	rm -f *.d
