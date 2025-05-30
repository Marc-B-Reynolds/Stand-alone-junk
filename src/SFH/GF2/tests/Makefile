# Dumb mini makefile:
# 0) assumes clang/GCC like options
# 1) every .c file is to be built into an executable

# if CC is the default (not environment varible nor supplied to make, then default
ifeq ($(origin CC),default)
  CC = clang-15
endif

IDIRS  = -I../.. -I..
CFLAGS = -g3 -O3 -flto ${IDIRS} -march=native -Wall -Wextra -Wconversion -Wpedantic -Wno-unused-function
LDLIBS = 
ODIR    := obj

# add filtering into group: m4ri, flint and neither
SRC     := ${wildcard *.c}
HEADERS := ${wildcard *.h}
TARGETS := ${SRC:.c=}
SIMDE   := ${SRC:.c=_simde}
DEPS    := ${addprefix ${ODIR}/, ${SRC:.c=.d}}

all:    ${TARGETS}

SIMDe:	${SIMDE}

spew:
	@echo ${SRC:.c=_simde}

.DEFAULT help:
	@echo "help            : there is no buildsystem"
	@echo " make           : builds all the test"
	@echo " make SIMDe     : builds all the test (with SIMDe)"
	@echo " make clean     : deletes all executables"
	@echo " make distclean : clean + kill emacs tempfiles and makedep file"

.PHONY: clean

clean:
	-${RM} ${TARGETS} ${SIMDE} ${DEPS}

distclean:	clean
	-${RM} *~

${ODIR}/%.d:%.c
	@-echo "building dependencies: " $<
	@-echo "# autogenerated by Makefile" > $@
	@mkdir -p ${ODIR}
	@$(CC) -MM -MQ${<:.c=} ${IDIRS} ${CFLAGS} $< >> $@

# anything with a flint suffix requires flint & m4ri
%_flint: %_flint.c Makefile
	${CC} ${CFLAGS} $< -L.. ${LDLIBS} -lflint -lm4ri -o $@

# anything with a m4ri suffix requires m4ri
%_m4ri_simde: %_m4ri.c Makefile
	${CC} -DSFH_USE_SIMDE ${CFLAGS} $< -L.. ${LDLIBS} -lm4ri -o $@

%_m4ri: %_m4ri.c Makefile
	${CC} ${CFLAGS} $< -L.. ${LDLIBS} -lm4ri -o $@

%_simde:%.c	Makefile
	${CC} -DSFH_USE_SIMDE ${CFLAGS} $< -L.. ${LDLIBS} -o $@

%:%.c	Makefile
	${CC} ${CFLAGS} $< -L.. ${LDLIBS} -o $@

-include ${DEPS}
