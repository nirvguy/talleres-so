CXX=g++
CXXFLAGS=-g -Wall
LDFLAGS=
BIN=test

OBJS=test.o

all: $(BIN)

$(BIN): $(OBJS)
	$(CXX) $(LDFLAGS) -o $@ $<

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -o $@ -c $<

clean:
	rm -rf $(OBJS) $(BIN)

.PHONY: clean
