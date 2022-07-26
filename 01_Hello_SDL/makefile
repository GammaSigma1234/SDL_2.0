# Makefile by Lazy Foo - Modificato da GS

# OBJS specifies which files to compile as part of the project
OBJS = 01_hello_SDL.cpp

# CC specifies which compiler we're using
CC = g++
# CC = D:\\MSYS64\\mingw64\\bin\\g++ # Non trova alcune librerie, perché MSYS2 non è nella variabile d'ambiente PATH di Windows

# INCLUDE_PATHS specifies the additional include paths we'll need (header files)
INCLUDE_PATHS = -ID:\Dati\SDL2-2.0.22\x86_64-w64-mingw32\include\SDL2

# LIBRARY_PATHS specifies the additional library paths we'll need (SDL2main and SDL2)
LIBRARY_PATHS = -LD:\Dati\SDL2-2.0.22\x86_64-w64-mingw32\lib

# COMPILER_FLAGS specifies the additional compilation options we're using
#  -Wl,-subsystem,windows gets rid of the console window
COMPILER_FLAGS = -Wl,-subsystem,windows

# LINKER_FLAGS specifies the (static) libraries we're linking against
LINKER_FLAGS = -lmingw32 -lSDL2main -lSDL2

# OBJ_NAME specifies the name of our exectuable
OBJ_NAME = 01_hello_SDL

# This is the target that compiles our executable
all : $(OBJS)
	$(CC) $(OBJS) $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(OBJ_NAME).exe