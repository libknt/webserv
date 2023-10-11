# SRCS = ../../../srcs/server/socket/socket.cpp ./socket_test.cc
SRCS= $(shell find ../../../srcs -name "socket.cpp")
SRCS+= ./socket_test.cc
SOCKETOUTPUT = socket_test

INCLUDE = -I $(dir $(shell find ../../../srcs -name "socket.hpp"))
all: socket
	./socket_test

socket: $(SRCS)
	$(CXX) $(CXXFLAGS) $(SRCS) $(INCLUDE) -o $(SOCKETOUTPUT) $(LDFLAGS)

clean:
	rm -f $(SOCKETOUTPUT)

.PHONY: socket clean

