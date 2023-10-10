SHELL := /bin/bash

format:
	find . -name '*.cpp' -or -name '*.hpp' -or "*.cc" | xargs clang-format -i

test:
	make -C test

cppcheck:
	mkdir -p log
	cppcheck --enable=all --error-exitcode=1 --inconclusive --force --suppress=unusedFunction --suppress=unmatchedSuppression --suppress=missingIncludeSystem --std=c++11 ./srcs/. 2> ./log/resule.log

tidy:
	@find . -name '*.cpp' -or -name '*.hpp' | while read file; do \
		clang-tidy $$file -- -Wall -Wextra -Werror -std=c++98; \
	done
