# program executable name
TARGET = testProgram

# compiler flags
CFLAGS = -g -std=gnu11 -Wall -fsanitize=address

# linker flags
LDFLAGS = -g -std=gnu11 -fsanitize=address

# list of sources
SOURCES = $(shell find -name "*.c")

# default rule, to compile everything
all: $(TARGET)

# define object files
OBJECTS = $(SOURCES:.c=.o)

# link programs
$(TARGET): $(OBJECTS)
	gcc $(LDFLAGS) -o $@ $^

# compile
%.o : %.c
	gcc $(CFLAGS) -c -o $@ $<
# cleaning

.PHONY: clean
clean:
	rm -f $(TARGET) $(OBJECTS)
