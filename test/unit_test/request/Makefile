NAME		=	Request_Unit_Test
CXX			=	c++
CXXFLAGS	=	-Wall -Werror -Wextra
VERSION		=	-std=c++14
INCLUDES	=	-I ./includes -I ${TESTPATH}
TESTPATH	=	../../../srcs/server/request
VPATH		=	${SRCSDIR}:${TESTPATH}
SRCS		=	${shell find ${SRCSDIR} -name "*.cpp" -type f}
SRCS		+=	${subst ${TESTPATH}, ${SRCSDIR}, ${shell find ${TESTPATH} -name "*.cpp" -type f}}
OBJS		=	${subst ${SRCSDIR}, ${OBJSDIR}, ${SRCS:.cpp=.o}}
SRCSDIR		=	srcs
OBJSDIR		=	objs
TESTDIR		=	test_case

all: ${NAME}

${NAME}: ${OBJS}
	${CXX} ${CXXFLAGS} ${VERSION} $^ -o $@

${OBJSDIR}/%.o:%.cpp
	mkdir -p ${dir ${OBJS}}
	${CXX} ${CXXFLAGS} ${VERSION} ${INCLUDES} -c $< -o $@

test: ${NAME}
	find ${TESTDIR} -type f | xargs ./${NAME}
clean:
	rm -rf ${OBJSDIR}
fclean: clean
	rm -rf ${NAME}
re: fclean all
