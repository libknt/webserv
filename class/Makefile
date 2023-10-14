all:
	c++ client.cpp ClientSocket.cpp -o client 
	c++ client2.cpp ClientSocket.cpp -o client2 

clean:
	rm -f server client client2

SRC =	$(shell find . -name '*.cpp' -or -name '*.hpp')

SHELL := /bin/bash

format:
	find . -name '*.cpp' -or -name '*.hpp' | xargs clang-format -i
