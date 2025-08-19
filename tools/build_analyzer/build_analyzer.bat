@echo off
REM uevr Cross-Engine VR Adapters - Build Analyzer (Batch Wrapper)
REM Analyzes builds for errors, warnings, and optimization opportunities
REM Copyright (c) 2024 uevr Cross-Engine Adapter Project

echo ========================================
echo uevr Build Analyzer
echo ========================================
echo.
echo This tool analyzes all build components for:
echo - Compilation errors and warnings
echo - Performance optimization opportunities
echo - Build script improvements
echo - Dependency analysis
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

REM Set analysis parameters
set ANALYSIS_MODE=comprehensive
set TARGET_COMPONENT=all
set GENERATE_REPORT=true
set AUTO_FIX=false
set PERFORMANCE_ANALYSIS=true
set DEPENDENCY_ANALYSIS=true

echo Analysis Configuration:
echo - Analysis Mode: %ANALYSIS_MODE%
echo - Target Component: %TARGET_COMPONENT%
echo - Generate Report: %GENERATE_REPORT%
echo - Auto-Fix Issues: %AUTO_FIX%
echo - Performance Analysis: %PERFORMANCE_ANALYSIS%
echo - Dependency Analysis: %DEPENDENCY_ANALYSIS%
echo.

REM Optional: Customize analysis
echo Customize analysis? (y/N)
set /p CUSTOMIZE="Enter choice: "

if /i "%CUSTOMIZE%"=="y" (
    echo.
    echo Analysis Mode Options:
    echo 1. comprehensive - Full analysis of all aspects
    echo 2. quick - Fast analysis focusing on critical issues
    echo 3. specific - Target specific component
    echo 4. performance - Focus on performance optimization
    echo 5. dependency - Focus on dependency management
    echo.
    set /p ANALYSIS_MODE="Enter analysis mode: "
    
    if /i "%ANALYSIS_MODE%"=="specific" (
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
    set /p AUTO_FIX="Enable auto-fix for common issues? (y/N): "
    if /i "%AUTO_FIX%"=="y" set AUTO_FIX=true
    
    echo.
    set /p PERFORMANCE_ANALYSIS="Enable performance analysis? (Y/n): "
    if /i "%PERFORMANCE_ANALYSIS%"=="n" set PERFORMANCE_ANALYSIS=false
    
    echo.
    set /p DEPENDENCY_ANALYSIS="Enable dependency analysis? (Y/n): "
    if /i "%DEPENDENCY_ANALYSIS%"=="n" set DEPENDENCY_ANALYSIS=false
)

echo.
echo Starting build analysis...
echo.

REM Run PowerShell analysis script
%PWSH_CMD% -ExecutionPolicy Bypass -File "%~dp0build_analyzer.ps1" -AnalysisMode %ANALYSIS_MODE% -TargetComponent %TARGET_COMPONENT% -GenerateReport:%GENERATE_REPORT% -AutoFix:%AUTO_FIX% -PerformanceAnalysis:%PERFORMANCE_ANALYSIS% -DependencyAnalysis:%DEPENDENCY_ANALYSIS%

REM Check exit code
if %errorlevel% neq 0 (
    echo.
    echo ========================================
    echo ANALYSIS FAILED
    echo ========================================
    echo Build analysis failed
    echo Check the logs in logs/analysis/ for details
    echo.
    pause
    exit /b 1
) else (
    echo.
    echo ========================================
    echo ANALYSIS COMPLETED
    echo ========================================
    echo Build analysis completed successfully
    echo Reports saved to: logs/analysis/
    echo.
    echo Review the analysis results for:
    echo - Build process issues
    echo - Performance optimization opportunities
    echo - Build script improvements
    echo - Dependency management
    echo.
)

pause
