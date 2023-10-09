CXX = c++

# kyodaのgoogle testのpath
# CXXFLAGS = -std=c++14 -I/sgoinfre/kyoda/googletest/include
# LDFLAGS = -L/sgoinfre/kyoda/googletest/lib -lgtest -lgtest_main

CXXFLAGS = -std=c++14 -I/usr/local/include
LDFLAGS = -L/usr/local/lib -lgtest -lgtest_main

SRCS = ../srcs/server/socket/Socket.cpp ./UnitTest/SocketTest/socket_test.cc

OUTPUT = test

all: socket
	./test

socket: $(SRCS)
	$(CXX) $(CXXFLAGS) -I ../srcs/server/socket/. $(SRCS) -o $(OUTPUT) $(LDFLAGS)

clean:
	rm -f $(OUTPUT)

.PHONY: socket clean

help:
	@echo
	@echo "find . -name '*.cpp' -or -name '*.hpp' | xargs clang-format -i"


