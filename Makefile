CC = g++
LD = g++
CFLAGS =  -O2 -Warning
LDFLAGS =  -lpthread
SRCS := $(wildcard *.cpp) # wildcard
OBJS = $(SRCS:.cpp=.o)
DEPS = $(SRCS:.cpp=.dep)
EXEC = $(SRCS:.cpp=)
RM = rm -f


all: DP1-1 DP1-2 DP1-3_main DP1-3_bench

DP1-1: DP1-1.o
	$(LD)  -o $@ $^ $(LDFLAGS)

DP1-2: DP1-2.o
	$(LD)  -o $@ $^ $(LDFLAGS)

DP1-3_main.o: DP1-3.hpp

DP1-3_main: DP1-3_main.o
	$(LD)  -o $@ $^ $(LDFLAGS)

DP1-3_bench.o: DP1-3.hpp

DP1-3_bench: DP1-3_bench.o
	$(LD)  -o $@ $^ $(LDFLAGS)

clean:
	$(RM) $(OBJS) $(EXEC) *~

check-syntax:
	$(CC) -Wall -Wextra -pedantic -fsyntax-only $(CHK_SOURCES)

.PHONY: clean 
	all clean
