.PHONY: test tidy format cppcheck
SHELL := /bin/bash

NAME		=	server

CXX			=	c++

CXXFLAGS	=	-Wall -Wextra -Werror -std=c++98

SRCS_DIR	=	srcs

OBJS_DIR	=	objs

INCLUDE =	-Iincludes/. -Isrcs/server/.

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


format:
	find . -name '*.cpp' -or -name '*.hpp' -or -name "*.cc" | xargs clang-format -i

test:
	make -C test

cppcheck:
	mkdir -p log
	cppcheck --enable=all --error-exitcode=1 --inconclusive --force --suppress=unusedFunction --suppress=unmatchedSuppression --suppress=missingIncludeSystem --std=c++11 -Iincludes ./srcs/. 2> ./log/resule.log

tidy:
	@find ./srcs/. -name '*.cpp' -or -name '*.hpp' | while read file; do \
		clang-tidy $$file -- -Wall -Wextra -Werror -std=c++98 >> ./log/tidy.log; \
	done
