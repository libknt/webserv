NAME       = Tcp_Socket_Unit_Test
CXX        = c++
CXXFLAGS   = -Wall -Werror -Wextra
VERSION    = -std=c++98
ROOT 	   = ./../../../
INCLUDES   = -I $(ROOT)/includes -I ${TESTPATH}
TESTPATH   = $(ROOT)/srcs/server/socket
VPATH      = ${SRCSDIR}:${TESTPATH}
SRCSDIR    = srcs
OBJSDIR    = objs
TESTDIR    = test_case

SRCS       = $(shell find ${SRCSDIR} -name "*.cpp" -type f)
SRCS       += $(subst ${TESTPATH},${SRCSDIR},$(shell find ${TESTPATH} -name "*.cpp" -type f))
OBJS       = $(subst ${SRCSDIR},${OBJSDIR},${SRCS:.cpp=.o})


all: ${NAME}

${NAME}: ${OBJS}
	${CXX} ${CXXFLAGS} ${VERSION} $^ -o $@

${OBJSDIR}/%.o:%.cpp
	mkdir -p $(dir $@)
	${CXX} ${CXXFLAGS} ${VERSION} ${INCLUDES} -c $< -o $@

test: ${NAME}
	./${NAME} 2> /dev/null
clean:
	rm -rf ${OBJSDIR}

fclean: clean
	rm -rf ${NAME}

re: fclean all