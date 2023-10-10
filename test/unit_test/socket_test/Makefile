SRCS = ../../../srcs/server/socket/socket.cpp ./socket_test.cc
SOCKETOUTPUT = socket_test

INCLUDE =  -I ../../../srcs/server/socket/.
all: socket
	./socket_test

socket: $(SRCS)
	$(CXX) $(CXXFLAGS) $(INCLUDE) $(SRCS) -o $(SOCKETOUTPUT) $(LDFLAGS)

clean:
	rm -f $(SOCKETOUTPUT)

.PHONY: socket clean

