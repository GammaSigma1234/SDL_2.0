@echo off
cls
echo.


if %1.==-c. goto Clean


@REM Setting path to SDL2 Development Tools
if not defined SDL_PATH (
  goto Path_Not_Defined
) else (
  goto Path_Defined
)

:Path_Not_Defined
  echo Setting path to SDL2 Development Tools...
  echo.
  set SDL_PATH=D:/Dati/SDL2-2.0.16/x86_64-w64-mingw32/
  goto Path_End

:Path_Defined
  echo Already set: %SDL_PATH%
  echo.

:Path_End


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


@REM Building executable
if exist 01_hello_SDL.exe (
  echo 01_hello_SDL.exe already exists. Deleting...
  echo.
  del 01_hello_SDL.exe
) else (
  echo.
)


echo Building...
echo.

g++ 01_hello_SDL.cpp -I%SDL_PATH%include/SDL2 -L%SDL_PATH%lib -w -Wl,-subsystem,windows -lmingw32 -lSDL2main -lSDL2 -o 01_hello_SDL
exit /b


:Clean
  echo Cleaning artifacts...
  echo.
  del 01_hello_SDL.exe
  echo Done.
  echo.