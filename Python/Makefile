CXX := g++
CXXFLAGS := -O3 -Wall -Wextra -pedantic -shared -fPIC

PYBIND11_FLAGS := $(shell python3 -m pybind11 --includes)
PYTHON_EXT := $(shell python3-config --extension-suffix)

SRC_DIR := ..
WRAPPER := uf26_wrapper.cc
TARGET := uf26$(PYTHON_EXT)
OBJS := $(SRC_DIR)/UF26Field.o

# This file is just the shared lib; assumes .o exists already
$(TARGET): $(WRAPPER) $(OBJS)
	$(CXX) $(CXXFLAGS) $(PYBIND11_FLAGS) -I$(SRC_DIR) -o $@ $^ $(shell python3-config --ldflags)

clean:
	rm -f $(TARGET)
