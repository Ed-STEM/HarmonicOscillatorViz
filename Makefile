UNAME_S := $(shell uname -s)
#OPTLIBS= -fsanitize=address // undefined // leak
#Header components, Flags, Sources, Libraries, and Locations to be compiled.
CC=clang 
###Extra c flags  -Isrc
CFLAGS=-g -target arm64-apple-macos11 -O2  -Wall -Wextra -D_FORTIFY_SOURCE=2 -fstack-protector-strong -Wformat-security  -fPIE  $(OPTFLAGS)
ifeq ($(UNAME_S),Darwin)
	INCLUDES=-I/opt/homebrew/opt/gsl/include -I/opt/homebrew/opt/raylib/include
	LDLIBS=-L/opt/homebrew/opt/gsl/lib -L/opt/homebrew/opt/raylib/lib -W1,-z,now$(OPTLIBS)
endif
ifeq ($(UNAME_S),Linux)
	INCLUDES=-I/usr/include/gsl
	LDLIBS=-L/usr/lib64 $(OPTLIBS)
endif
LIBS=-lgsl -lgslcblas -lm -lraylib$(OPTLIBS)

SOURCES=$(wildcard src/**/*.c src/*.c)
OBJECTS=$(patsubst %.c,%.o,$(SOURCES))

TEST_SRC=$(wildcard tests/*_tests.c)
TESTS=$(patsubst %.c,%,$(TEST_SRC))

###Change TARGET
TARGET=build/ho_viz
SO_TARGET=$(patsubst %.a,%.so,$(TARGET))

###Change EXECUTABLE
EXECUTABLE=bin/ho_viz

#Target Build Components and Flag options
.PHONY: all dev clean install
all: $(TARGET) $(SO_TARGET) $(EXECUTABLE) 

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $(OBJECTS)  $(LDLIBS) $(LIBS)
	chmod +x $(EXECUTABLE)

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o $@ $<

dev: CFLAGS=-g -Wall  -Isrc -Wall -Wextra $(OPTFLAGS)
dev: all

$(TARGET): CFLAGS += -fPIC
$(TARGET): build $(OBJECTS) 
	ar rcs $@ $(OBJECTS)
	ranlib $@
$(SO_TARGET): $(TARGET) $(OBJECTS)
	$(CC) -shared -o $@ $(OBJECTS) $(INCLUDES) $(LDLIBS) $(LIBS)

debug: $(TARGET)
ifeq ($(UNAME_S),Darwin)
	lldb ./$(TARGET)
endif
ifeq ($(UNAME_S),Linux)
	gdb ./$(TARGET)
endif
	

memcheck: $(TARGET)
ifeq ($(UNAME_S),Darwin)
	leaks --atExit ./$(TARGET)
endif
ifeq ($(UNAME_S),Linux)
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./$(TARGET)
endif
	

build:
	@mkdir -p build
	@mkdir -p bin

#Cleaning 
clean:
	rm -rf build $(OBJECTS) $(TESTS) $(EXECUTABLE) 
	find . -name "*.gc*" -exec rm {} \;
	rm -rf `find . -name "*.dSYM" -print`

#Install
install: all
	install -d $(DESTDIR)/$(PREFIX)/lib/
	install $(TARGET) $(DESTDIR)/$(PREFIX)/lib/