SHELL := /bin/bash

NAME		=	server

CXX			=	c++

CXXFLAGS	=	-Wall -Wextra -Werror -std=c++98

ADDRESS_FLAGS = -g -fsanitize=address

LEAK_FLAGS= -g -fsanitize=leak

SRCS_DIR	=	srcs

OBJS_DIR	=	objs

INCLUDE = -I includes/. $(addprefix -I , $(dir $(shell find srcs -name "*.hpp")))

SRCS		=	$(shell find $(SRCS_DIR) -name "*.cpp")

OBJS		=	$(subst $(SRCS_DIR), $(OBJS_DIR), $(SRCS:.cpp=.o))

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $@

$(OBJS_DIR)/%.o: $(SRCS_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDE) -o $@ -c $<
	
clean:
	rm -rf $(OBJS_DIR)

fclean: clean
	$(RM) $(NAME)

re: fclean all


docker:
	@echo docker run -it --rm -v $$PWD:/root/webserv ubuntu:latest /bin/bash
	@echo apt-get update
	@echo apt install -y netbase g++ vim make

format:
	find . -name '*.cpp' -or -name '*.hpp' -or -name "*.cc" | xargs clang-format -i

test:
	make -C test

address: fclean
address: CXXFLAGS += $(ADDRESS_FLAGS)
address: all

leak: fclean
leak: CXXFLAGS += $(LEAK_FLAGS)
leak: all

.PHONY: test tidy format cppcheck

cppcheck:
	mkdir -p log
	cppcheck --enable=all --check-config --error-exitcode=1 --inconclusive --force --suppress=missingIncludeSystem --std=c++11 $(INCLUDE) $(SRCS). 2> ./log/resule.log

tidy:
	@find ./srcs/. -name '*.cpp' -or -name '*.hpp' | while read file; do \
		clang-tidy $$file -- -Wall -Wextra -Werror -std=c++98 $(INCLUDE) >> ./log/tidy.log; \
	done
