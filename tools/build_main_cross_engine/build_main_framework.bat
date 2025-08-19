@echo off
REM uevr Main Cross-Engine Framework Build Script (Batch Wrapper)
REM Calls the PowerShell build script with proper error handling
REM Copyright (c) 2024 uevr Cross-Engine Adapter Project

echo ========================================
echo uevr Main Cross-Engine Framework Build
echo ========================================
echo.

REM Check if PowerShell is available
where pwsh >nul 2>nul
if %errorlevel% neq 0 (
    where powershell >nul 2>nul
    if %errorlevel% neq 0 (
        echo ERROR: PowerShell not found
        echo Please install PowerShell Core (pwsh) or Windows PowerShell
        pause
        exit /b 1
    ) else (
        set PWSH_CMD=powershell
    )
) else (
    set PWSH_CMD=pwsh
)

echo Using PowerShell: %PWSH_CMD%
echo.

REM Set build parameters
set BUILD_TYPE=Release
set WARNINGS_AS_ERRORS=true
set PARALLEL_JOBS=8

echo Build Configuration:
echo - Build Type: %BUILD_TYPE%
echo - Warnings as Errors: %WARNINGS_AS_ERRORS%
echo - Parallel Jobs: %PARALLEL_JOBS%
echo.

REM Run PowerShell build script
echo Starting build...
%PWSH_CMD% -ExecutionPolicy Bypass -File "%~dp0build_main_framework.ps1" -BuildType %BUILD_TYPE% -WarningsAsErrors:%WARNINGS_AS_ERRORS% -ParallelJobs %PARALLEL_JOBS%

REM Check exit code
if %errorlevel% neq 0 (
    echo.
    echo ========================================
    echo BUILD FAILED
    echo ========================================
    echo Check the logs in logs/build/main_cross_engine/ for details
    echo.
    pause
    exit /b 1
) else (
    echo.
    echo ========================================
    echo BUILD SUCCESSFUL
    echo ========================================
    echo Main cross-engine framework built successfully
    echo Logs saved to: logs/build/main_cross_engine/
    echo.
)

pause
