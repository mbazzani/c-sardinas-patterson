# Compiler and compiler flags
CC = g++
CFLAGS = -Wall -Wextra -std=c++11 -ofast -march=native -msse4.2

# Source files and executable name
SOURCE_FILES = main.cpp
EXECUTABLE = out

# Default target
all: $(EXECUTABLE)

# Rule to build the executable
$(EXECUTABLE): $(SOURCE_FILES)
	$(CC) $(CFLAGS) $^ -o $@

# Clean rule to remove the executable and any intermediate files
clean:
	rm -f $(EXECUTABLE)

fmt:
	clang-format -i $(SOURCE_FILES)

