CFLAGS=-g -Wall -rdynamic -DNDEBUG $(OPTFLAGS)
LIBS=-ldl $(OPTLIBS)
PREFIX?=/usr/local

SOURCES=$(wildcard src/**/*.c src/*.c)
OBJECTS=$(patsubst %.c, %.o, $(SOURCES))

TEST_SRC=$(wildcard tests/*_tests.c)
TESTS=$(patsubst %.c, %.o, $(TEST_SRC))

TARGET=bin/server

all: $(TARGET)

$(TARGET): Build $(OBJECTS)
		$(CC) -o $@ $(OBJECTS)

Build:
		@mkdir -p build
		@mkdir -p bin

.PHONY: tests
#tests: CFLAGS += $(TARGET)
#tests: $(TESTS)
		sh ./tests/runtests/sh

valgrind:
	VALGRIND="valgrind --log-file=/tmp/valgrind-%p.log" $(MAKE)

clean:
	rm -rf bin $(OBJECTS) $(TESTS)
	rm -f tests/tests.log

install: all
		install -d $(DESTDIR)/$(PREFILX)/lib/
		install $(TARGET) $(DESTDIR)/$(PREFIX)/lib/
