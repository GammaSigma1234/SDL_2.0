@REM By GS - Settembre 2021 - Rivisitato Lug 2022

@echo off
cls
echo.

@REM Set temporary environment variables
set SDL2_PROJECT_NAME=05_optimized_surface_loading_and_soft_stretching
set SDL2_INCLUDE_PATH=D:\Dati\SDL2\SDL2-2.0.22\x86_64-w64-mingw32\include\SDL2
set SDL2_LIB_PATH=D:\Dati\SDL2\SDL2-2.0.22\x86_64-w64-mingw32\lib
set SDL2_LIBRARIES=-lmingw32 -lSDL2main -lSDL2

@REM Assign global variables from TXT file (not used any more)
@REM for /f "delims== tokens=1,2 skip=2" %%G in (..\Global_Variables_For_Batch_Files.txt) do set %%G=%%H


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

g++ %SDL2_PROJECT_NAME%.cpp -I%SDL2_INCLUDE_PATH% -L%SDL2_LIB_PATH% %SDL2_LINKER_OPTIONS% %SDL2_LIBRARIES% -o %SDL2_PROJECT_NAME%.exe

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