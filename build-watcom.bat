@echo off
REM Build script for Open Watcom project
REM Runs owsetenv.bat before building
REM Usage: build-watcom.bat [clean|rebuild]
REM Requires: WATCOM_ROOT and DOS_INSTALL_DIR environment variables

if not defined WATCOM_ROOT (
    echo Error: WATCOM_ROOT environment variable not set
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

if not defined WATCOM (
    echo Setting up Open Watcom environment...
    call "%WATCOM_ROOT%\owsetenv.bat"
) else (
    echo Re-using existing Open Watcom environment...
)

if not defined WATCOM (
    echo Error: Open Watcom environment not set up correctly
    exit /b 1
)

REM Create directories if they don't exist
if not exist obj mkdir obj
if not exist bin mkdir bin

echo Compiling source files...
set COMPILE_ERROR=0
for %%f in (src\*.c) do (
    echo   Compiling %%f...
    wcc386 -mf -4 -w4 -ox -ot -ol+ -oi -fo=obj\%%~nf.obj %%f
    if errorlevel 1 set COMPILE_ERROR=1
)

if %COMPILE_ERROR%==1 (
    echo Compilation failed!
    exit /b 1
)

echo Linking...
wlink system dos4g file {obj\*.obj} name bin\main.exe

if errorlevel 1 (
    echo Linking failed!
    exit /b 1
)

echo Build successful! Output: bin\main.exe

REM Install to target directory
if not exist "%DOS_INSTALL_DIR%" mkdir "%DOS_INSTALL_DIR%"
echo Installing bin\main.exe to %DOS_INSTALL_DIR%...
copy bin\main.exe "%DOS_INSTALL_DIR%" > nul
echo Installation complete.
goto :eof

:clean
echo Cleaning build artifacts...
if exist obj\*.obj del obj\*.obj
if exist bin\main.exe del bin\main.exe
echo Clean complete.
if "%1"=="clean" goto :eof
exit /b 0
