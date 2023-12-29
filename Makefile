# OBJS specifies which files to compile as part of the project
OBJS = main.cpp

# CC specifies which compiler we're using
CC = g++

# COMPILER_FLAGS = -w

LINKER_FLAGS = -lsfml-graphics -lsfml-window -lsfml-system

OBJ_NAME = output

all : $(OBJS)
	$(CC) $(OBJS) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(OBJ_NAME)
