.PHONY: test tidy format cppcheck

SHELL := /bin/bash
# SRCS = $(shell find srcs -name "*.cpp")
SRCS = $(shell find srcs -name "main.cpp")

NAME = a.out

all:
	cc  $(SRCS) -o $(NAME)

clean:
	rm -f a.out

re: clean all

format:
	find . -name '*.cpp' -or -name '*.hpp' -or -name "*.cc" | xargs clang-format -i

test:
	make -C test

cppcheck:
	mkdir -p log
	cppcheck --enable=all --error-exitcode=1 --inconclusive --force --suppress=unusedFunction --suppress=unmatchedSuppression --suppress=missingIncludeSystem --std=c++11 ./srcs/. 2> ./log/resule.log

tidy:
	@find . -name '*.cpp' -or -name '*.hpp' | while read file; do \
		clang-tidy $$file -- -Wall -Wextra -Werror -std=c++98; \
	done
