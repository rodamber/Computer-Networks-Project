CXXFLAGS=-Wall -Wextra -pedantic -g
CXX=g++

EXEC=TES
SOURCES=$(wildcard *.cpp)
OBJECTS=$(SOURCES:.cpp=.o)
HEADERS=$(SOURCES:.cpp=.hpp)

run: all
	./$(EXEC)

all: $(EXEC)

$(EXEC): $(OBJECTS) utils.o
	$(CXX) $(CXXFLAGS) $(OBJECTS) utils.o -o $(EXEC)

-include $(OBJECTS:.o=.d)

%.o: %.cpp
	$(CXX) -MMD -c $(CXXFLAGS) -o $@ $<

clean:
	rm -f $(EXEC) $(OBJECTS)

cleand:
	rm -f *.d
