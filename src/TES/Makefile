CXXFLAGS=-Wall -Wextra -pedantic -g
CXX=g++

EXEC=TES
SOURCES=$(wildcard *.c)
OBJECTS=$(SOURCES:.c=.o)
HEADERS=$(SOURCES:.c=.h)

run: all
	./$(EXEC)

all: $(EXEC)

$(EXEC): $(OBJECTS) utils.o
	$(CXX) $(CXXFLAGS) $(OBJECTS) utils.o -o $(EXEC)

-include $(OBJECTS:.o=.d)

%.o: %.c
	$(CXX) -MMD -c $(CXXFLAGS) -o $@ $<

clean:
	rm -f $(EXEC) $(OBJECTS)

cleand:
	rm -f *.d
