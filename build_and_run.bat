@echo off
title TrueCare Hospital Management System - C Web Engine
cls
echo ====================================================================
echo        TRUECARE HOSPITAL MANAGEMENT SYSTEM - C WEB ENGINE
echo ====================================================================
echo.

:: Check common MinGW GCC installation locations if not already in PATH
where gcc >nul 2>&1
if %ERRORLEVEL% NEQ 0 (
    if exist "%USERPROFILE%\w64devkit\bin\gcc.exe" (
        set "PATH=%USERPROFILE%\w64devkit\bin;%PATH%"
    ) else if exist "C:\w64devkit\bin\gcc.exe" (
        set "PATH=C:\w64devkit\bin;%PATH%"
    ) else if exist "C:\MinGW\bin\gcc.exe" (
        set "PATH=C:\MinGW\bin;%PATH%"
    ) else if exist "C:\Program Files\CodeBlocks\MinGW\bin\gcc.exe" (
        set "PATH=C:\Program Files\CodeBlocks\MinGW\bin;%PATH%"
    )
)

echo Checking GCC Compiler...
gcc --version >nul 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo [!] GCC Compiler was not found in your system PATH.
    echo Please ensure GCC (MinGW or Code::Blocks) is installed.
    pause
    exit /b %ERRORLEVEL%
)

echo [OK] GCC Compiler found!
echo Compiling main.c with Windows Sockets (-lws2_32)...
gcc main.c -o hospital.exe -lws2_32

if %ERRORLEVEL% EQU 0 (
    echo.
    echo ====================================================================
    echo [SUCCESS] Build complete! Launching C Web Application...
    echo Browser will open automatically at: http://localhost:8080
    echo ====================================================================
    echo.
    hospital.exe
) else (
    echo.
    echo [!] Compilation failed. Please check main.c.
    pause
)
