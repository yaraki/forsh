# Makefile for forsh

COMPILER = clang
SOURCES = stack.c value.c context.c map.c builtin.c error.c
TEST_SOURCES = $(wildcard *_test.c)
TESTS = $(patsubst %.c,%,$(TEST_SOURCES))
OBJECTS = $(patsubst %.c,%.o,$(SOURCES))
TARGET = forsh

all: $(TARGET)
%.o: %.c
	$(COMPILER) -c $<
$(TARGET): main.c $(OBJECTS)
	$(COMPILER) -o $@ $^
clean:
	rm -f $(TARGET) $(OBJECTS) $(TESTS)
test: $(TESTS)
	./$^
%_test: %_test.c $(OBJECTS)
	$(COMPILER) -o $@ $^
