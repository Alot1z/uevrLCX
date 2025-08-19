@echo off
REM uevr RE Engine Adapter Build Script (Batch Wrapper)
REM Builds Resident Evil 7 VR adapter with full cross-engine integration
REM Copyright (c) 2024 uevr Cross-Engine Adapter Project

echo ========================================
echo uevr RE Engine Adapter Build
echo ========================================
echo.
echo Target: Resident Evil 7 (RE Engine)
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
echo - Target: Resident Evil 7 VR adapter
echo.

REM Run PowerShell build script
echo Starting RE Engine adapter build...
%PWSH_CMD% -ExecutionPolicy Bypass -File "%~dp0build_re_engine.ps1" -BuildType %BUILD_TYPE% -WarningsAsErrors:%WARNINGS_AS_ERRORS% -ParallelJobs %PARALLEL_JOBS%

REM Check exit code
if %errorlevel% neq 0 (
    echo.
    echo ========================================
    echo BUILD FAILED
    echo ========================================
    echo RE Engine adapter build failed
    echo Check the logs in logs/build/re_engine/ for details
    echo.
    pause
    exit /b 1
) else (
    echo.
    echo ========================================
    echo BUILD SUCCESSFUL
    echo ========================================
    echo RE Engine adapter built successfully
    echo Deployed to: profiles/re7/
    echo Logs saved to: logs/build/re_engine/
    echo.
    echo Ready for Resident Evil 7 VR support!
    echo.
)

pause
