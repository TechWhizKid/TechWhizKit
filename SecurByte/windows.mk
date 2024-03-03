# Define the compiler and the compiler flags
CC = gcc
CFLAGS = -Wall -O2

# Define the source files and the object files
SRC = main.c aes.c sha256.c
OBJ = $(SRC:.c=.o)

# Define the executable file name
EXE = SecurByte

# Define the default rule that runs when make is invoked without arguments
all: $(EXE)

# Define the rule that links the object files into the executable file
$(EXE): $(OBJ)
	$(CC) $(CFLAGS) -o $(EXE) $(OBJ)

# Define the rule that compiles each source file into an object file
%.o: %.c
	$(CC) $(CFLAGS) -c $<

# Define the rule that cleans the intermediate and output files
clean:
	rm -f $(OBJ) $(EXE)
