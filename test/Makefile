CXX = c++

# kyoda mac
# CXXFLAGS = -std=c++14 -I/sgoinfre/kyoda/googletest/include -Wall -Werror -Wextra
# LDFLAGS = -L/sgoinfre/kyoda/googletest/lib -lgtest -lgtest_main

# kyoda ubuntu
# CXXFLAGS = -std=c++14 -I/usr/local/include
# LDFLAGS = -L/usr/local/lib -lgtest -lgtest_main


CXXFLAGS = -std=c++14 -I/usr/local/include -Wall -Werror -Wextra
LDFLAGS = -L/usr/local/lib -lgtest -lgtest_main

all:socket

socket:
	make -C UnitTest/SocketTest CXX="$(CXX)" CXXFLAGS="$(CXXFLAGS)" LDFLAGS="$(LDFLAGS)"
	make clean -C UnitTest/SocketTest
