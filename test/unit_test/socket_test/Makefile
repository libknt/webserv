# SRCS = ../../../srcs/server/socket/socket.cpp ./socket_test.cc
SRCS= $(shell find ../../../srcs -name "socket.cpp")
SRCS+= ./socket_test.cc
OUTPUT = socket_test

INCLUDE = -I $(dir $(shell find ../../../srcs -name "socket.hpp"))
INCLUDE += -I ../../../includes
all: socket
	./socket_test

socket: $(SRCS)
	$(CXX) $(CXXFLAGS) $(SRCS) $(INCLUDE) -o $(OUTPUT) $(LDFLAGS)

clean:
	rm -f $(OUTPUT)

.PHONY: socket clean

