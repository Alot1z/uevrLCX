@echo off
REM Batch file to run the uevr to uevr conversion script
REM This will change all instances of "uevr" to "uevr" throughout the codebase

echo.
echo ========================================
echo   uevr to uevr Conversion Script
echo ========================================
echo.
echo This script will change all instances of "uevr" to "uevr"
echo throughout the entire codebase.
echo.
echo WARNING: This will modify multiple files!
echo Make sure you have committed your changes or have a backup.
echo.
pause

echo.
echo Starting conversion...
echo.

REM Run the PowerShell script
powershell -ExecutionPolicy Bypass -File "%~dp0change_uevr_to_lowercase.ps1"

echo.
echo Conversion completed!
echo.
pause
