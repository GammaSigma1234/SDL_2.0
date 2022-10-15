@echo off

cls
echo.

@REM Project's name
set SDL2_PROJECT_NAME=Calculator

@REM Source files
set CLASSES_DIR=.\Classes
set SOURCE_FILES=main.cpp

@REM Static libraries paths
set SDL2_LIB_PATH=D:\Dati\SDL2\SDL2-2.0.22\x86_64-w64-mingw32\lib
set SDL2_IMAGE_LIB_PATH=D:\Dati\SDL2\SDL2_image-2.6.0\x86_64-w64-mingw32\lib\
set SDL2_TTF_LIB_PATH=D:\Dati\SDL2\SDL2_ttf-2.20.0\x86_64-w64-mingw32\lib\
set SDL2_LIB_PATHS=-L%SDL2_LIB_PATH% -L%SDL2_IMAGE_LIB_PATH% -L%SDL2_TTF_LIB_PATH%

@REM Static libraries
set SDL2_LIBRARIES=-lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf

@REM Header files
set SDL2_INCLUDE_PATH=D:\Dati\SDL2\SDL2-2.0.22\x86_64-w64-mingw32\include\SDL2
set SDL2_IMAGE_INCLUDE_PATH=D:\Dati\SDL2\SDL2_image-2.6.0\x86_64-w64-mingw32\include\SDL2\
set SDL2_TTF_INCLUDE_PATH=D:\Dati\SDL2\SDL2_ttf-2.20.0\x86_64-w64-mingw32\include\SDL2
set CLASSES_INCLUDE_PATH=.\Classes
set INTERFACES_INCLUDE_PATH=.\Interfaces
set COLOURS_LIB_INCLUDE_PATH=D:\Dati\Versionamento\Git\Esercizi_Programmazione\SDL_2.0\Colours_Lib
set SDL2_INCLUDE_PATHS=-I%SDL2_INCLUDE_PATH% -I%SDL2_IMAGE_INCLUDE_PATH% -I%SDL2_TTF_INCLUDE_PATH% -I%CLASSES_INCLUDE_PATH% -I%INTERFACES_INCLUDE_PATH% -I%COLOURS_LIB_INCLUDE_PATH%

@REM C++ compilation options
set COMPILATION_OPTIONS=-Wall -Wextra -Wpedantic -Wconversion


if %1.==-c. goto Clean


@REM Build executable
if exist %SDL2_PROJECT_NAME%.exe (
  echo %SDL2_PROJECT_NAME%.exe already exists. Deleting...
  echo.
  del %SDL2_PROJECT_NAME%.exe
) else (
  echo.
)


echo Building executable...
echo.

@REM echo on
g++ %COMPILATION_OPTIONS% %CLASSES_DIR%\*.cpp %SOURCE_FILES% %SDL2_INCLUDE_PATHS% %SDL2_LIB_PATHS% %SDL2_LIBRARIES% -o %SDL2_PROJECT_NAME%.exe
@REM echo off

IF %ERRORLEVEL% EQU 0 (
  echo.
  echo [32mCompilation was successful![0m
  echo.
) else (
  echo.
  echo [31m*** Error ***[0m
  echo.
  echo Compilation errors were encountered.
)
exit /b


:Clean
  echo Cleaning artifacts...
  echo.
  del %SDL2_PROJECT_NAME%.exe
  echo Done.
  echo.