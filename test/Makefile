CXX = c++

GPWD=$(shell pwd)
CXXFLAGS = -std=c++14 -Wall -Werror -Wextra -I$(GPWD)/third_party/gtest/include
LDFLAGS = -L$(GPWD)/third_party/gtest/lib -lgtest -lgtest_main

# all: check_wget check_third_party socket 
all: check_wget check_third_party socket io_multiplexing

check_wget:
	@if ! command -v wget &> /dev/null; then $(MAKE) install_wget; exit 1; fi

install_wget:
	@echo "\033[1;31mwget is not installed. Please install wget using one of the following methods:\033[0m";
	@echo "\033[1;31mbrew install wget\033[0m";
	@echo "\033[1;31mapt install wget\033[0m";
	@exit 1

check_third_party:
	@if [ ! -d "./third_party/gtest" ]; then $(MAKE) gpath; fi


socket:
	make -C unit_test/socket_test CXX="$(CXX)" CXXFLAGS="$(CXXFLAGS)" LDFLAGS="$(LDFLAGS)"
	make clean -C unit_test/socket_test

io_multiplexing:
	make -C unit_test/io_multiplexing_test CXX="$(CXX)" CXXFLAGS="$(CXXFLAGS)" LDFLAGS="$(LDFLAGS)"
	make clean -C unit_test/io_multiplexing_test

gpath:
	bash build.sh

action-build:check_third_party
action: socket io_multiplexing