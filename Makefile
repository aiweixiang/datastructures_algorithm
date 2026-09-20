# Build and run the test suites for every chapter.
CC      ?= cc
CFLAGS  ?= -std=c11 -Wall -Wextra -O2 -g -I. -Iinclude
LDLIBS  := -lm

CH02_LIB := $(wildcard ch02_algorithm_analysis/*.c)
CH03_LIB := $(wildcard ch03_lists_stacks_queues/*.c)

TESTS := build/test_ch02 build/test_ch03

.PHONY: all test demos clean

all: $(TESTS)

build:
	@mkdir -p build

build/test_ch02: tests/test_ch02.c $(CH02_LIB) include/test_util.h | build
	$(CC) $(CFLAGS) -o $@ tests/test_ch02.c $(CH02_LIB) $(LDLIBS)

build/test_ch03: tests/test_ch03.c $(CH03_LIB) include/test_util.h | build
	$(CC) $(CFLAGS) -o $@ tests/test_ch03.c $(CH03_LIB) $(LDLIBS)

# the original hand-written demo kept from the first draft
build/kth_largest_demo: demos/kth_Largest.c | build
	$(CC) $(CFLAGS) -o $@ $<

test: all
	@for t in $(TESTS); do ./$$t || exit 1; done

demos: build/kth_largest_demo

clean:
	rm -rf build
