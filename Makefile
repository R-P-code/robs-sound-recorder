#Object files
OBJS = glob_vars.cpp robs_sound_recorder.cpp gui_functions.cpp recorder_functions.cpp


#Compiler
CC = g++


COMPILER_FLAGS = `pkg-config --cflags gtk+-3.0`


LINKER_FLAGS = `pkg-config --libs gtk+-3.0` -lSDL2 -lSDL2_mixer


OBJ_NAME = rsr


all : $(OBJS)
	$(CC) $(OBJS) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(OBJ_NAME)
