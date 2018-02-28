VERSION=$(shell cat VERSION.txt)
#Compiler
CC = gcc -O3 -DVERSION='"$(VERSION)"' -g

#compiler flags
# -g adds debug info to the executable file
# -Wall turns on most warnings from compiler
CFLAGS = -Wall

HTSLOC?=$(HTSLIB)

HTSTMP?=./htslib_tmp
prefix=?/usr/local/

#Define locations of header files
OPTINC?= -I$(HTSLOC)/
INCLUDES= -I./ $(OPTINC) -rdynamic

# define library paths in addition to /usr/lib
#   if I wanted to include libraries not in /usr/lib I'd specify
#   their path using -Lpath, something like:
LFLAGS?= -L$(HTSTMP)

# define any libraries to link into executable:
#   if I want to link in libraries (libx.so or libx.a) I use the -llibname
#   option, something like (this will link in libmylib.so and libm.so:
LIBS =-lhts -lpthread -lz -lbz2 -llzma -lm -ldl

# define the C source files
SRCS = ./c/context.c ./c/bed_access.c
#Define test sources
TEST_SRC=$(wildcard ./c_tests/*_tests.c)
TESTS=$(patsubst %.c,%,$(TEST_SRC))

# define the C object files
#
# This uses Suffix Replacement within a macro:
#   $(name:string1=string2)
#         For each word in 'name' replace 'string1' with 'string2'
# Below we are replacing the suffix .c of all words in the macro SRCS
# with the .o suffix
#
OBJS = $(SRCS:.c=.o)

MD := mkdir

#Build target executable
CONTEXT_COUNTER_TARGET=bin/context_counter

#
# The following part of the makefile is generic; it can be used to
# build any executable just by changing the definitions above and by
# deleting dependencies appended to the file from 'make depend'
#

.PHONY: depend clean test make_htslib_tmp remove_htslib_tmp pre

.NOTPARALLEL: test

#all: clean pre make_htslib_tmp $(TRIPLETS_TARGET) test remove_htslib_tmp
all: clean pre make_htslib_tmp $(CONTEXT_COUNTER_TARGET) remove_htslib_tmp
	@echo triplets compiled.

$(CONTEXT_COUNTER_TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(INCLUDES) -o $(CONTEXT_COUNTER_TARGET) $(OBJS) $(LFLAGS) $(LIBS)
#	$(CC) $(CFLAGS) $(INCLUDES) -o $(TRIPLETS_TARGET) $(OBJS) $(LFLAGS) $(LIBS) ./c/triplets.o

#Unit Tests
test: $(CONTEXT_COUNTER_TARGET)
test: CFLAGS += $(INCLUDES) $(OBJS) $(LFLAGS) $(LIBS)
test: $(TESTS)
	sh ./c/c_tests/runtests.sh

make_htslib_tmp:
	$(MD) $(HTSTMP)
	#Do some magic to ensure we compile BAM_STATS with the static libhts.a rather than libhts.so
	ln -s $(HTSLOC)/libhts.a $(HTSTMP)/libhts.a

remove_htslib_tmp:
	@echo remove tmp hts location
	-rm -rf $(HTSTMP)

copyscript:
	chmod a+x $(TRIPLETS_TARGET)

valgrind:
	VALGRIND="valgrind --log-file=/tmp/valgrind-%p.log" $(MAKE)


# this is a suffix replacement rule for building .o's from .c's
# it uses automatic variables $<: the name of the prerequisite of
# the rule(a .c file) and $@: the name of the target of the rule (a .o file)
# (see the gnu make manual section about automatic variables)
.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $<  -o $@

clean:
	@echo clean
	$(RM) ./c/*.o *~ $(CONTEXT_COUNTER_TARGET) ./tests/tests_log $(TESTS) ./c/*.gcda ./c/*.gcov ./c/*.gcno *.gcda *.gcov *.gcno ./c/tests/*.gcda ./c/tests/*.gcov ./c/tests/*.gcno
	-rm -rf $(HTSTMP)

depend: $(SRCS)
	makedepend $(INCLUDES) $^

# DO NOT DELETE THIS LINE -- make depend needs it
