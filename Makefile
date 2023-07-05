CC = gcc
CFLAGS = -g -Wall -Wextra

# List of source files
SRC = main.c hamming.c chunk.c

# List of object files
OBJ = $(SRC:.c=.o)
# Replace main.o with test.o
TEST_OBJ = $(filter-out main.o, $(OBJ)) test.o


# Main target
hamming: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

# Rule for building object files from source files
%.o: %.c %.h
	$(CC) $(CFLAGS) -c $< -o $@

# Rule for building and running the tests
test: $(TEST_OBJ)
	$(CC) $(CFLAGS) -o $@ $^

# Cleanup rule
clean:
	rm -f hamming test *.o