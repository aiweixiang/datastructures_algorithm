# 编译并运行每一章的测试。
CC      ?= cc
CFLAGS  ?= -std=c11 -Wall -Wextra -O2 -g -I. -Iinclude
LDLIBS  := -lm

CH01_LIB := $(wildcard ch01_introduction/*.c)
CH02_LIB := $(wildcard ch02_algorithm_analysis/*.c)
CH03_LIB := $(wildcard ch03_lists_stacks_queues/*.c)

TESTS := build/test_ch01 build/test_ch02 build/test_ch03

.PHONY: all test demos clean

all: $(TESTS)

build:
	@mkdir -p build

build/test_ch01: tests/test_ch01.c $(CH01_LIB) include/test_util.h | build
	$(CC) $(CFLAGS) -DFIXTURE_DIR='"tests/fixtures"' -o $@ tests/test_ch01.c $(CH01_LIB) $(LDLIBS)

build/test_ch02: tests/test_ch02.c $(CH02_LIB) include/test_util.h | build
	$(CC) $(CFLAGS) -o $@ tests/test_ch02.c $(CH02_LIB) $(LDLIBS)

build/test_ch03: tests/test_ch03.c $(CH03_LIB) include/test_util.h | build
	$(CC) $(CFLAGS) -o $@ tests/test_ch03.c $(CH03_LIB) $(LDLIBS)

# 手写 demo 与计时程序（含 main，不参与测试链接）
build/kth_largest_demo: demos/kth_Largest.c | build
	$(CC) $(CFLAGS) -o $@ $<

build/selection_timing: demos/selection_timing.c $(CH02_LIB) | build
	$(CC) $(CFLAGS) -o $@ demos/selection_timing.c $(CH02_LIB) $(LDLIBS)

test: all
	@for t in $(TESTS); do ./$$t || exit 1; done

demos: build/kth_largest_demo build/selection_timing

clean:
	rm -rf build
