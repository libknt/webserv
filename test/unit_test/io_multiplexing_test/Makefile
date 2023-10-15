SRCSSERVER= main.cc
SRCSSERVER+= $(shell find ../../../srcs -name "server.cpp")
SRCSSERVER+= $(shell find ../../../srcs -name "socket.cpp")
SRCSSERVER+= $(shell find ../../../srcs -name "io_multiplexing.cpp")

SRCS=io_multiplexing_test.cc

INCLUDE = -I $(dir $(shell find ../../../srcs -name "socket.hpp"))

re: fclean all

all: server client io_multiplexing test

io_multiplexing:
	$(CXX) $(CXXFLAGS) $(SRCS) $(INCLUDE) -o io_multiplexing_test $(LDFLAGS)

test:
	./io_multiplexing_test
	
server: $(SRCSSERVER)
	$(CXX) $(CXXFLAGS) $(SRCSSERVER) $(INCLUDE) -o server $(LDFLAGS)

client:
	$(MAKE) -C clientTest
	cp ./clientTest/client ./

clean:
	rm -f server client io_multiplexing_test
	make clean -C clientTest

fclean: clean


.PHONY: io_multiplexing clean
