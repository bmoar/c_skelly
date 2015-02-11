#name of your project:
LIB_NAME=cskelly
CC=gcc

# OS detection and associated options:
UNAME := $(shell uname)
ifeq ($(UNAME), Linux)
	TESTSCRIPT=./test/runtests.sh
	MEMTEST=valgrind --track-origins=yes --leak-check=full --show-reachable=yes
	PLAT_LIBS= -ldl -lrt -lm -pthread
	EXE_EXT=
	SO_EXT=.so
endif
ifeq ($(UNAME), MINGW32_NT-6.1)
	TESTSCRIPT=sh ./test/runtests.sh
	MEMTEST=drmemory -batch
	PLAT_LIBS=
	EXE_EXT=.exe
	SO_EXT=.dll
endif


CFLAGS=-g -fPIC -O2 -Wall -Wextra -Isrc -Iinclude -DNDEBUG $(OPTFLAGS)
LIBS=$(INCS) $(PLAT_LIBS) $(OPTLIBS)
PREFIX?=/usr/local

HEADERS=$(wildcard src/**/*.h src/*.h)
SOURCES=$(wildcard src/**/*.c src/*.c)
OBJECTS=$(patsubst %.c,%.o,$(SOURCES))

TEST_H=$(wildcard test/**/*.h test/*.h)
TEST_SRC=$(wildcard test/**/*_test.c test/*_test.c)
TEST_OBJ=$(patsubst %.c,%.o,$(TEST_SRC))
TESTS=$(patsubst %.o,%,$(TEST_OBJ))

INC_H=$(wildcard include/**/*.h include/*.h)
INC_SRC=$(wildcard include/**/*.c include/*.c)
INC_OBJ=$(patsubst %.c,%.o,$(INC_SRC))
INCS=$(patsubst %.o,%.a,$(INC_OBJ))

PROGRAMS_SRC=$(wildcard bin/*.c)
PROGRAMS=$(patsubst %.c,%,$(PROGRAMS_SRC))

TARGET=build/lib$(LIB_NAME).a
SO_TARGET=$(patsubst %.a,%$(SO_EXT),$(TARGET))

# The Target Build
all: $(TARGET) $(SO_TARGET) $(PROGRAMS) test 

dev: CFLAGS=-g -fPIC -O2 -Wall -Wextra -Isrc -Iinclude $(OPTFLAGS) 
dev: all

$(TARGET): build $(HEADERS) $(OBJECTS)
	ar rcs $@ $(OBJECTS)
	ranlib $@

$(SO_TARGET): $(TARGET) $(OBJECTS)
	$(CC) -shared -o $@ $(OBJECTS) $(LIBS)

build:
	@mkdir -p build
	@mkdir -p bin

include: $(INCS)

$(INCS): $(INC_H) $(INC_OBJ)
	ar rcs $@ $(INC_OBJ)
	ranlib $@
	
# The Unit Tests
.PHONY: test
test: CFLAGS += $(TESTFLAGS)
test: $(TESTS) 
	@touch ./test/tests.log
	@TEST_PAT=*_test$(EXE_EXT) $(TESTSCRIPT)

$(TESTS): $(TEST_OBJ) $(TARGET)
	$(CC) -o $@ $(patsubst %,%.o,$@) $(TARGET) $(LIBS)

$(TEST_OBJ): $(TEST_H)

$(PROGRAMS): %.o: %.c
	$(CC) $@.c -static $(CFLAGS) -l$(LIB_NAME) -Lbuild $(PLAT_LIBS) -o $@

memtest:
	MEMTEST="$(MEMTEST)" $(MAKE) dev

# The Cleaner
clean:
	rm -rf build 
	rm -f $(OBJECTS) $(wildcard test/*.exe test/*.o)
	rm -f test/tests.log test/tests.log.old
	rm -rf build $(OBJECTS) $(TESTS) $(PROGRAMS)
#find . -name "*.gc*" -exec rm {} \;
#rm -rf 'find . -name "*.dSYM" -print'

# The Install
install: all
	install -d $(DESTDIR)/$(PREFIX)/lib/
	install $(TARGET) $(DESTDIR)/$(PREFIX)/lib/

# The Checker
BADFUNCS='[^_.>a-zA-Z0-9] (str(n?cpy|n?cat|xfrm|n?dup|str|pbrk|tok|_)|stpn?cpy|a?sn?printf|byte_)'
check:
	@echo Files with potentially dangerous functions:
	grep $(BADFUNCS) $(SOURCES) || true

