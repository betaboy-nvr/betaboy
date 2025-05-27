# Compiler and Flags
CC = gcc
CFLAGS = -Wall -Wextra -g -std=c99
LDFLAGS = -lncurses

# Source files, Object files, and Target executable for the main game
SRCS = main.c tetris.c
OBJS = $(SRCS:.c=.o) # main.o tetris.o
TARGET = tetris

# Source files, Object files, and Target executable for tests
TEST_MAIN_SRC = test_tetris.c
TEST_CORE_SRC = tetris.c # Core logic being tested
TEST_SRCS = $(TEST_MAIN_SRC) $(TEST_CORE_SRC) # test_tetris.c tetris.c

# Object files for the test executable: test_tetris.o and tetris.o
# tetris.o can be reused from the main build if dependencies are compatible.
TEST_OBJS = $(TEST_MAIN_SRC:.c=.o) $(TEST_CORE_SRC:.c=.o) # test_tetris.o tetris.o
TEST_TARGET = tetris_test

# Default rule: build the main game executable
all: $(TARGET)

# Rule to link main game object files into the target executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LDFLAGS)

# Generic rule to compile C source files into object files
# Depends on the source file itself and tetris.h
# This will create main.o, tetris.o, and test_tetris.o as needed
%.o: %.c tetris.h
	$(CC) $(CFLAGS) -c $< -o $@

# --- Test Target ---
test: $(TEST_TARGET)
	./$(TEST_TARGET)

# Rule to link test object files into the test executable
$(TEST_TARGET): $(TEST_OBJS)
	# LDFLAGS might not be strictly needed if tetris.c's core logic doesn't call ncurses.
	# However, including it for safety if any indirect dependencies exist or if tetris.h includes ncurses types.
	$(CC) $(CFLAGS) -o $(TEST_TARGET) $(TEST_OBJS) $(LDFLAGS)

# Rule to clean up build artifacts (main game and tests)
clean:
	rm -f $(TARGET) $(OBJS) $(TEST_TARGET) $(TEST_MAIN_SRC:.c=.o) # Removes main target, main.o, tetris.o, test_target, test_tetris.o
	# tetris.o is already covered by $(OBJS) if SRCS includes tetris.c

# Phony targets (targets that are not actual files)
.PHONY: all clean test
