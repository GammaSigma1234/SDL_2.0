@echo off

cls
echo Starting program in %cd%...
echo.

@REM Set temporary environment variables
set SDL2_PROJECT_NAME=38_particle_engines
set SDL2_DLL=D:\Dati\SDL2\SDL2-2.0.22\x86_64-w64-mingw32\bin
set SDL2_IMAGE_DLL=D:\Dati\SDL2\SDL2_image-2.6.0\x86_64-w64-mingw32\bin
set SDL2_TTF_DLL=D:\Dati\SDL2\SDL2_ttf-2.20.0\x86_64-w64-mingw32\bin\
set SDL2_DLL_PATHS=%SDL2_DLL%;%SDL2_IMAGE_DLL%;%SDL2_TTF_DLL%

@REM Check whether path to SDL2 dynamic libraries is already present
if not defined PATH_TO_DLL_IS_PRESENT (
  goto Path_SDL2_Add
) else (
  goto Path_SDL2_No_Add
)

:Path_SDL2_Add
  echo Adding path to dynamic libraries...
  echo.
  SET PATH=%PATH%;%SDL2_DLL_PATHS%;
  SET PATH_TO_DLL_IS_PRESENT=1
  goto Path_SDL2_End

:Path_SDL2_No_Add
  echo Path to dynamic libraries already added. Skipping...
  echo.

:Path_SDL2_End


if exist %SDL2_PROJECT_NAME%.exe (
  goto Executable_Found
) else (
  goto Executable_Not_Found
)

:Executable_Found
  echo Executable %SDL2_PROJECT_NAME%.exe found. Executing...
  start "" "%SDL2_PROJECT_NAME%"
  echo.
  goto Executable_End

:Executable_Not_Found
  echo [31m***ERROR***[0m
  echo.
  echo Executable [31m%SDL2_PROJECT_NAME%.exe[0m not found. Compile with [32mBuild.bat[0m first.
  echo.

:Executable_End
exit /b
