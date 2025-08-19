@echo off
REM uevr MT Framework Adapter Build Script (Batch Wrapper)
REM Builds Monster Hunter World VR adapter with full cross-engine integration
REM Copyright (c) 2024 uevr Cross-Engine Adapter Project

echo ========================================
echo uevr MT Framework Adapter Build
echo ========================================
echo.
echo Target: Monster Hunter World (MT Framework)
echo Purpose: Build VR adapter with cross-engine integration
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
echo - Target: Monster Hunter World VR adapter
echo.

REM Run PowerShell build script
echo Starting MT Framework adapter build...
%PWSH_CMD% -ExecutionPolicy Bypass -File "%~dp0build_mt_framework.ps1" -BuildType %BUILD_TYPE% -WarningsAsErrors:%WARNINGS_AS_ERRORS% -ParallelJobs %PARALLEL_JOBS%

REM Check exit code
if %errorlevel% neq 0 (
    echo.
    echo ========================================
    echo BUILD FAILED
    echo ========================================
    echo MT Framework adapter build failed
    echo Check the logs in logs/build/mt_framework/ for details
    echo.
    pause
    exit /b 1
) else (
    echo.
    echo ========================================
    echo BUILD SUCCESSFUL
    echo ========================================
    echo MT Framework adapter built successfully
    echo Deployed to: profiles/MonsterHunterWorld/
    echo Logs saved to: logs/build/mt_framework/
    echo.
    echo Ready for Monster Hunter World VR support!
    echo.
)

pause
