@echo off
title CareWell Hospital Management System - C Web Engine
cls
echo ====================================================================
echo        CAREWELL HOSPITAL MANAGEMENT SYSTEM - C WEB ENGINE
echo ====================================================================
echo.

:: Ensure MinGW GCC is in the current session PATH
set "PATH=C:\Users\mvdev\w64devkit\bin;%PATH%"

echo Checking GCC Compiler...
gcc --version >nul 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo [!] Error: GCC compiler not found in C:\Users\mvdev\w64devkit\bin
    pause
    exit /b %ERRORLEVEL%
)

echo [OK] MinGW GCC 13.2.0 is available!
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
