# Name of the project
PROJ_NAME=pmm

# headers directory
HDIR=header
# source code directory
CDIR=src
# build directory ( where the object files will be stored )
ODIR=build

# .c files
C_SOURCE=$(wildcard ./$(CDIR)/*.c)

# .h files
H_SOURCE=$(wildcard ./$(HDIR)/*.h)

# Object files
OBJ=$(subst .c,.o,$(subst $(CDIR),$(ODIR),$(C_SOURCE)))

# Compiler
CC=gcc

# Flags for compiler
CC_FLAGS=-I						\
		 -Wall					\
		 -Wextra				\
		 -Wshadow				\
		 -Wundef				\
		 -Wformat=2				\
		 -Wfloat-equal			\
		 -Wcast-align			\
		 -std=c17				\
		 -march=native			\
		 -O3					\
		 -Ofast					\
		 -g

# Libraries
LIBS=-lm

#
# Compilation and linking
#
all: objFolder $(PROJ_NAME)

$(PROJ_NAME): $(OBJ)
	$(CC) -o $@ $^ $(CC_FLAGS) $(LIBS)

./$(ODIR)/%.o: ./$(CDIR)/%.c ./$(HDIR)/%.h
	$(CC) -c -o $@ $< $(CC_FLAGS) $(LIBS)

./$(ODIR)/main.o: ./$(CDIR)/main.c $(H_SOURCE)
	$(CC) -c -o $@ $< $(CC_FLAGS) $(LIBS)

objFolder:
	@ mkdir -p $(ODIR)

.PHONY: run

run:
	@ read -r -p "Enter the path to the file to compile: " PATH \
		&& ./$(PROJ_NAME) $${PATH};

.PHONY: clean

clean:
	@ rm -rf ./$(ODIR)/*.o ./$(ODIR) $(PROJ_NAME)

.PHONY: valgrind
valgrind:
	@ read -r -p "Enter the path to the file to compile: " PATH \
		&& /usr/bin/valgrind --leak-check=full ./$(PROJ_NAME) $${PATH};
