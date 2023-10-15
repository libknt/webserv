.PHONY: test tidy format cppcheck

SHELL := /bin/bash

NAME = server

CXX		= c++

CXXFLAGS	= -Wall -Wextra -Werror -std=c++98

SRC =	$(shell find ./srcs -name "*.cpp")


OBJ = $(SRC:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJ)
	$(CXX) $(CXXFLAGS) $^ -o $@

clean:
	$(RM) $(OBJ)

fclean: clean
	$(RM) $(NAME)

re: fclean all








format:
	find . -name '*.cpp' -or -name '*.hpp' -or -name "*.cc" | xargs clang-format -i

test:
	make -C test

cppcheck:
	mkdir -p log
	cppcheck --enable=all --error-exitcode=1 --inconclusive --force --suppress=unusedFunction --suppress=unmatchedSuppression --suppress=missingIncludeSystem --std=c++11 ./srcs/. 2> ./log/resule.log

tidy:
	@find ./srcs/. -name '*.cpp' -or -name '*.hpp' | while read file; do \
		clang-tidy $$file -- -Wall -Wextra -Werror -std=c++98; \
	done
