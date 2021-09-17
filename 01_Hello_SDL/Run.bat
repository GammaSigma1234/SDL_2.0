@echo off
echo Starting program in %cd%...
echo.


@REM Setting path to SDL2.dll dynamic library
if not defined PATH_DLL_IS_OK (
  goto Path_SDL2_Add
) else (
  goto Path_SDL2_No_Add
)

:Path_SDL2_Add
  echo Adding path to SDL2.dll...
  echo.
  SET PATH=%PATH%;D:/Dati/SDL2-2.0.16/x86_64-w64-mingw32/bin
  SET PATH_DLL_IS_OK=1
  goto Path_SDL2_End

:Path_SDL2_No_Add
  echo Path to SDL2.dll already added
  echo.

:Path_SDL2_End


if exist 01_hello_SDL.exe (
  goto Executable_Found
) else (
  goto Executable_Not_Found
)

:Executable_Found
  echo Executable found. Executing...
  start "" "01_hello_SDL.exe"
  goto Executable_End

:Executable_Not_Found
  echo Executable not found. Compile with Build.bat first.

:Executable_End
exit /b
