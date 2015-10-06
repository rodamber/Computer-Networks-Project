CC = gcc
CCFLAGS = -Wall -Wextra -pedantic

EXEC = run_all_tests
SOURCES = $(wildcard *.c) ../src/utils.c
OBJECTS = $(SOURCES:.c=.o)
HEADERS = ../src/utils.h

all: $(EXEC)

run: all
	./$(EXEC)

$(EXEC): $(OBJECTS) $(HEADERS)
	$(CC) ${CCFLAGS} $(OBJECTS) -o $(EXEC)

%.o: %.c $(HEADERS)
	${CC} ${CCFLAGS} -c -g -o $@ $<

clean:
	$(RM) $(EXEC) $(OBJECTS)
