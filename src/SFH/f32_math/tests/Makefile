# Dumb mini makefile:
# 1) every .c file is to be built into an executable
# 2) doesn't check dependancy on headers. oh well.

CFLAGS = -g -O3 -I../.. -I.. -march=native -Wall -Wextra -Wconversion -Wpedantic -Wno-unused-function -fno-math-errno -ffp-contract=off
LDLIBS = -lm

SRC     := ${wildcard *.c}
HEADERS := ${wildcard *.h}
TARGETS := ${SRC:.c=}

all:    ${TARGETS}

clean:
	-${RM} ${TARGETS}

distclean:	clean
	-${RM} *~
