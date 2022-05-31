# C compiler	
CC=gcc			

DEFS=

CFLAGS= $(DEFS) -g -Wall 

# executable name
EXEC = compiler

# LIBS=-lm
LIBS = 

# headers directory
HDIR = header
# source code directory
CDIR = src
# build directory ( where the object files will be stored )
ODIR = build

# list of header files
_DEPS = lexer.h parser.h vector.h
DEPS = $(patsubst %,$(HDIR)/%,$(_DEPS))

# list of source code files ( replacing the .c/.cpp extension with .o )
_OBJ = main.o lexer.o parser.o vector.o
OBJ = $(patsubst %, $(ODIR)/%,$(_OBJ))

# used to create the build directory if it doesn't exist
dir_guard = @mkdir -p $(@D)

# rule for files ending in .o
$(ODIR)/%.o: $(CDIR)/%.c $(DEPS)	
	$(dir_guard)
	$(CC) -c -o $@ $< $(CFLAGS)

# "make all" to compile
.PHONY: all
all: $(OBJ)
	$(CC) -o $(EXEC) $^ $(CFLAGS) $(LIBS)

# "make run" to execute
.PHONY: run
run:
	./$(EXEC)

# "make debug" to execute with debugger
.PHONY: debug
debug:
	gdb ./$(EXEC)

# "make valgrind" to execute with valgrind (memory leak detection)
.PHONY: valgrind
valgrind:
	valgrind --leak-check=full ./$(EXEC)

# "make clean" to remove object files
.PHONY: clean
clean:
	rm -f $(ODIR)/*.o  
