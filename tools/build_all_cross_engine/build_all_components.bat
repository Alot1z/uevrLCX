@echo off
REM uevr Cross-Engine VR Adapters - Master Build Script (Batch Wrapper)
REM Builds all 4 components: Main Framework + 3 Engine Adapters
REM Copyright (c) 2024 uevr Cross-Engine Adapter Project

echo ========================================
echo uevr Cross-Engine VR Adapters - Master Build
echo ========================================
echo.
echo This script will build ALL 4 components:
echo 1. Main Cross-Engine Framework
echo 2. MT Framework Adapter (Monster Hunter World)
echo 3. RE Engine Adapter (Resident Evil 7)
echo 4. REDengine 4 Adapter (Cyberpunk 2077)
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

REM Optional: Skip specific components
echo Optional: Skip specific components?
set /p SKIP_MAIN="Skip Main Framework? (y/N): "
set /p SKIP_MT="Skip MT Framework? (y/N): "
set /p SKIP_RE="Skip RE Engine? (y/N): "
set /p SKIP_RED="Skip REDengine 4? (y/N): "

REM Build skip flags
set SKIP_FLAGS=
if /i "%SKIP_MAIN%"=="y" set SKIP_FLAGS=%SKIP_FLAGS% -SkipMainFramework
if /i "%SKIP_MT%"=="y" set SKIP_FLAGS=%SKIP_FLAGS% -SkipMTFramework
if /i "%SKIP_RE%"=="y" set SKIP_FLAGS=%SKIP_FLAGS% -SkipREEngine
if /i "%SKIP_RED%"=="y" set SKIP_FLAGS=%SKIP_FLAGS% -SkipREDengine4

echo.
echo Starting master build with configuration:
echo - Build Type: %BUILD_TYPE%
echo - Warnings as Errors: %WARNINGS_AS_ERRORS%
echo - Parallel Jobs: %PARALLEL_JOBS%
if defined SKIP_FLAGS echo - Skip Flags: %SKIP_FLAGS%
echo.

REM Run master PowerShell build script
echo Starting master build...
%PWSH_CMD% -ExecutionPolicy Bypass -File "%~dp0build_all_components.ps1" -BuildType %BUILD_TYPE% -WarningsAsErrors:%WARNINGS_AS_ERRORS% -ParallelJobs %PARALLEL_JOBS%%SKIP_FLAGS%

REM Check exit code
if %errorlevel% neq 0 (
    echo.
    echo ========================================
    echo MASTER BUILD FAILED
    echo ========================================
    echo Some components failed to build
    echo Check the logs in logs/build/all_components/ for details
    echo Individual component logs available in their respective directories
    echo.
    pause
    exit /b 1
) else (
    echo.
    echo ========================================
    echo MASTER BUILD SUCCESSFUL
    echo ========================================
    echo All requested components built successfully!
    echo.
    echo Deployed Components:
    echo - Main Framework: include/build/
    echo - MT Framework: profiles/MonsterHunterWorld/
    echo - RE Engine: profiles/re7/
    echo - REDengine 4: profiles/Cyberpunk2077/
    echo.
    echo Logs saved to: logs/build/all_components/
    echo.
    echo 🎉 uevr Cross-Engine VR Adapter system is ready! 🎉
    echo.
)

pause
