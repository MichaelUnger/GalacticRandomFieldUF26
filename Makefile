CXX := g++
CXXFLAGS := -Wall -Wextra -pedantic -O3

EXE := $(patsubst %.cxx, %, $(wildcard *.cxx))
TESTS := $(patsubst %.cxx, %, $(wildcard Test/test*.cxx))
SRCS := $(wildcard *.cc)
OBJS := $(patsubst %.cc,%.o,$(SRCS))

all: $(EXE)

%: %.cxx $(OBJS)
	$(CXX) -o $@ $^ $(CXXFLAGS)

%.o: %.cc
	$(CXX) $(CXXFLAGS) -fPIC -c $<

test: $(TESTS)
	./Test/testUF26Field Test/referenceValues.txt

pybind: ./UF26Field.o
	$(MAKE) -C Python

clean:
	rm -rf $(EXE) $(TESTS) *.o
	$(MAKE) -C Python clean

.PRECIOUS: %.o
.PHONY: all test pybind clean
