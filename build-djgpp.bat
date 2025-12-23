@echo off
REM Build script for DJGPP project
REM Runs setenv.bat before building
REM Usage: build-djgpp.bat [clean|rebuild]
REM Requires: DJGPP_ROOT and DOS_INSTALL_DIR environment variables

if not defined DJGPP_ROOT (
    echo Error: DJGPP_ROOT environment variable not set
    exit /b 1
)

if not defined DOS_INSTALL_DIR (
    echo Error: DOS_INSTALL_DIR environment variable not set
    exit /b 1
)

REM Handle clean command
if "%1"=="clean" goto clean
if "%1"=="rebuild" (
    call :clean
    goto build
)

:build

if not defined DJDIR (
    echo Setting up DJGPP environment...
    call "%DJGPP_ROOT%\setenv.bat"
) else (
    echo Re-using existing DJGPP environment...
)

if not defined DJDIR (
    echo Error: DJGPP environment not set up correctly
    exit /b 1
)

REM Create directories if they don't exist
if not exist obj mkdir obj
if not exist bin mkdir bin

echo Compiling source files...
set COMPILE_ERROR=0
for %%f in (src\*.c) do (
    echo   Compiling %%f...
    gcc -O2 -Wall -c %%f -o obj\%%~nf.o
    if errorlevel 1 set COMPILE_ERROR=1
)

if %COMPILE_ERROR%==1 (
    echo Compilation failed!
    exit /b 1
)

echo Linking...
gcc -o bin\dmain.exe obj\*.o -lm

if errorlevel 1 (
    echo Linking failed!
    exit /b 1
)

echo Build successful! Output: bin\dmain.exe

REM Install to target directory
if not exist "%DOS_INSTALL_DIR%" mkdir "%DOS_INSTALL_DIR%"
echo Installing bin\dmain.exe to %DOS_INSTALL_DIR%...
copy bin\dmain.exe "%DOS_INSTALL_DIR%" > nul
echo Copying assets to %DOS_INSTALL_DIR%...
copy assets\*.* "%DOS_INSTALL_DIR%" > nul
echo Installation complete.
goto :eof

:clean
echo Cleaning build artifacts...
if exist obj\*.o del obj\*.o
if exist bin\dmain.exe del bin\dmain.exe
echo Clean complete.
if "%1"=="clean" goto :eof
exit /b 0
