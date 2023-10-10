CXX = c++

GPWD=$(shell pwd)
CXXFLAGS = -std=c++14 -Wall -Werror -Wextra -I$(GPWD)/third_party/gtest/include
LDFLAGS = -L$(GPWD)/third_party/gtest/lib -lgtest -lgtest_main

all: check_third_party socket

check_third_party:
	@if [ ! -d "./third_party/gtest" ]; then $(MAKE) gpath; fi

socket:
	make -C UnitTest/SocketTest CXX="$(CXX)" CXXFLAGS="$(CXXFLAGS)" LDFLAGS="$(LDFLAGS)"
	make clean -C UnitTest/SocketTest

gpath:
	bash build.sh

action: socket