all:
	c++ client.cpp ClientSocket.cpp -o client 

clean:
	rm -f server client

SRC =	$(shell find . -name '*.cpp' -or -name '*.hpp')

SHELL := /bin/bash

format:
	find . -name '*.cpp' -or -name '*.hpp' | xargs clang-format -i
