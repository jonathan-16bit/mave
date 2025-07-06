# Compiler and flags
CC = gcc
CFLAGS = -std=c11 -Wall -Iheaders -g

# Source and output
SRC = src/main.c src/commitfuncs.c src/lcsfuncs.c src/strfuncs.c
OUT = program

# Default target: build the program
all: $(OUT)

# Linking rule
$(OUT): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(OUT)

# Run target: build if needed, then run
run: $(OUT)
	./$(OUT)

# Clean target: remove the executable
clean:
	rm -f $(OUT)
