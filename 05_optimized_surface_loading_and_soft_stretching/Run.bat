@REM By GS - Settembre 2021 - Rivisitato Lug 2022

@echo off

cls
echo Starting program in %cd%...
echo.


@REM Set temporary environment variables
set SDL2_PROJECT_NAME=05_optimized_surface_loading_and_soft_stretching
set SDL2_DLL_LATEST_VER_PATH=D:\Dati\SDL2\SDL2-2.0.22\x86_64-w64-mingw32\bin

@REM Assign global variables from TXT file
@REM for /f "delims== tokens=1,2 skip=2" %%G in (..\Global_Variables_For_Batch_Files.txt) do set %%G=%%H

@REM Check whether path to SDL2.dll dynamic library is already present
if not defined PATH_TO_DLL_IS_PRESENT (
  goto Path_SDL2_Add
) else (
  goto Path_SDL2_No_Add
)

:Path_SDL2_Add
  echo Adding path to SDL2.dll...
  echo.
  SET PATH=%PATH%;%SDL2_DLL_LATEST_VER_PATH%
  SET PATH_TO_DLL_IS_PRESENT=1
  goto Path_SDL2_End

:Path_SDL2_No_Add
  echo Path to SDL2.dll already added. Skipping...
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
