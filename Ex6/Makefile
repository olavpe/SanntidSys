# program executable name
TARGET = testProgram

# compiler flags
CFLAGS = -g -Wall -std=gnu11

# linker flags
LDFLAGS = -g -lpthread -lcomedi -lm

# list of sources
SOURCES = $(shell find -name "*.c")

# default rule, to compile everything
all: $(TARGET)

# define object files
OBJECTS = $(SOURCES:.c=.o)

# link programs
$(TARGET): $(OBJECTS)
	gcc -o  $@ $^ $(LDFLAGS)

# compile
%.o : %.c
	gcc $(CFLAGS) -c -o $@ $<
# cleaning

.PHONY: clean
clean:
	rm -f $(TARGET) $(OBJECTS)