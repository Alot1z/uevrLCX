@echo off
REM uevr Cross-Engine VR Adapters - Build Optimizer (Batch Wrapper)
REM Optimizes build scripts and processes for better performance and quality
REM Copyright (c) 2024 uevr Cross-Engine Adapter Project

echo ========================================
echo uevr Build Optimizer
echo ========================================
echo.
echo This tool optimizes all build components for:
echo - Build performance improvements
echo - Compiler flag optimization
echo - Parallel build optimization
echo - Dependency optimization
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

REM Set optimization parameters
set OPTIMIZATION_MODE=comprehensive
set TARGET_COMPONENT=all
set BACKUP_ORIGINALS=true
set APPLY_OPTIMIZATIONS=true
set PERFORMANCE_TESTING=true
set GENERATE_REPORT=true

echo Optimization Configuration:
echo - Optimization Mode: %OPTIMIZATION_MODE%
echo - Target Component: %TARGET_COMPONENT%
echo - Backup Originals: %BACKUP_ORIGINALS%
echo - Apply Optimizations: %APPLY_OPTIMIZATIONS%
echo - Performance Testing: %PERFORMANCE_TESTING%
echo - Generate Report: %GENERATE_REPORT%
echo.

REM Optional: Customize optimization
echo Customize optimization? (y/N)
set /p CUSTOMIZE="Enter choice: "

if /i "%CUSTOMIZE%"=="y" (
    echo.
    echo Optimization Mode Options:
    echo 1. comprehensive - Full optimization of all aspects
    echo 2. performance - Focus on build speed and efficiency
    echo 3. quality - Focus on build quality and warnings
    echo 4. specific - Target specific component
    echo 5. conservative - Safe optimizations only
    echo.
    set /p OPTIMIZATION_MODE="Enter optimization mode: "
    
    if /i "%OPTIMIZATION_MODE%"=="specific" (
        echo.
        echo Component Options:
        echo 1. all - All components (Main, MT, RE, RED)
        echo 2. main - Main Cross-Engine Framework only
        echo 3. mt - MT Framework only
        echo 4. re - RE Engine only
        echo 5. red - REDengine 4 only
        echo.
        set /p TARGET_COMPONENT="Enter target component: "
    )
    
    echo.
    set /p BACKUP_ORIGINALS="Backup original files before optimization? (Y/n): "
    if /i "%BACKUP_ORIGINALS%"=="n" set BACKUP_ORIGINALS=false
    
    echo.
    set /p APPLY_OPTIMIZATIONS="Apply optimizations automatically? (Y/n): "
    if /i "%APPLY_OPTIMIZATIONS%"=="n" set APPLY_OPTIMIZATIONS=false
    
    echo.
    set /p PERFORMANCE_TESTING="Run performance testing after optimization? (Y/n): "
    if /i "%PERFORMANCE_TESTING%"=="n" set PERFORMANCE_TESTING=false
)

echo.
echo Starting build optimization...
echo.

REM Run PowerShell optimization script
%PWSH_CMD% -ExecutionPolicy Bypass -File "%~dp0build_optimizer.ps1" -OptimizationMode %OPTIMIZATION_MODE% -TargetComponent %TARGET_COMPONENT% -BackupOriginals:%BACKUP_ORIGINALS% -ApplyOptimizations:%APPLY_OPTIMIZATIONS% -PerformanceTesting:%PERFORMANCE_TESTING% -GenerateReport:%GENERATE_REPORT%

REM Check exit code
if %errorlevel% neq 0 (
    echo.
    echo ========================================
    echo OPTIMIZATION FAILED
    echo ========================================
    echo Build optimization failed
    echo Check the logs in logs/optimization/ for details
    echo.
    pause
    exit /b 1
) else (
    echo.
    echo ========================================
    echo OPTIMIZATION COMPLETED
    echo ========================================
    echo Build optimization completed successfully
    echo Reports saved to: logs/optimization/
    echo.
    echo Review the optimization results for:
    echo - Build performance improvements
    echo - Compiler flag optimizations
    echo - Parallel build enhancements
    echo - Dependency optimizations
    echo.
)

pause
