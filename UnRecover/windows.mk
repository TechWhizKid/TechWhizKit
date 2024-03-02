# Define the compiler
CC = gcc

# Define the source file
SRC = main.c

# Define the executable file
EXE = UnRecover.exe

# Define the rule to build the executable file
$(EXE): $(SRC)
	$(CC) $(SRC) -o $(EXE)
