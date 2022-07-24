@REM By GS - Settembre 2021 - Rivisitato Lug 2022

@echo off

@REM Assign global variables from TXT file
for /f "delims== tokens=1,2 skip=2" %%G in (..\Global_Variables_For_Batch_Files.txt) do set %%G=%%H

cls
echo.


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

g++ %SDL2_PROJECT_NAME%.cpp -I%SDL2_LATEST_VER_ROOT%\%SDL2_INCLUDE_PATH% -L%SDL2_LATEST_VER_ROOT%\%SDL2_LIB_PATH% -w %SDL2_LINKER_OPTIONS% %SDL2_LIBRARIES% -o %SDL2_PROJECT_NAME%.exe

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