@echo off

cls
echo.

@REM Set temporary environment variables
set SDL2_PROJECT_NAME=18_key_states

@REM Static libraries
set SDL2_LIB_PATH=D:\Dati\SDL2\SDL2-2.0.22\x86_64-w64-mingw32\lib
set SDL2_IMAGE_LIB_PATH=D:\Dati\SDL2\SDL2_image-2.6.0\x86_64-w64-mingw32\lib\
set SDL2_TTF_LIB_PATH=D:\Dati\SDL2\SDL2_ttf-2.20.0\x86_64-w64-mingw32\lib\
set SDL2_LIBRARIES=-lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf

@REM Header files
set SDL2_INCLUDE_PATH=D:\Dati\SDL2\SDL2-2.0.22\x86_64-w64-mingw32\include\SDL2
set SDL2_IMAGE_INCLUDE_PATH=D:\Dati\SDL2\SDL2_image-2.6.0\x86_64-w64-mingw32\include\SDL2\
set SDL2_TTF_INCLUDE_PATH=D:\Dati\SDL2\SDL2_ttf-2.20.0\x86_64-w64-mingw32\include\SDL2

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

g++ %COMPILATION_OPTIONS% %SDL2_PROJECT_NAME%.cpp -I%SDL2_INCLUDE_PATH% -I%SDL2_IMAGE_INCLUDE_PATH% -I%SDL2_TTF_INCLUDE_PATH% -L%SDL2_LIB_PATH% -L%SDL2_IMAGE_LIB_PATH% %SDL2_LIBRARIES% -L%SDL2_TTF_LIB_PATH% -o %SDL2_PROJECT_NAME%.exe

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