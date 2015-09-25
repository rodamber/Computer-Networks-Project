CC = gcc
CCFLAGS = -Wall -Wextra -pedantic

EXEC = run_all_tests
SOURCES = $(wildcard *.c)
OBJECTS = $(SOURCES:.c=.o)

$(EXEC): $(OBJECTS)
	$(CC) ${CCFLAGS} $(OBJECTS) -o $(EXEC)

%.o: %.c
	${CC} ${CCFLAGS} -c -g -o $@ $<

clean:
	$(RM) $(EXEC) $(OBJECTS)
