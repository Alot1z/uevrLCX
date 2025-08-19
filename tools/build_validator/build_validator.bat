@echo off
REM uevr Cross-Engine VR Adapters - Build Validator (Batch Wrapper)
REM Validates all builds and generates comprehensive QA reports
REM Copyright (c) 2024 uevr Cross-Engine Adapter Project

echo ========================================
echo uevr Build Validator
echo ========================================
echo.
echo This tool validates all build components and generates:
echo - Build validation reports
echo - Deployment verification
echo - Dependency checks
echo - QA validation reports
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

REM Set validation parameters
set VALIDATION_MODE=comprehensive
set TARGET_COMPONENT=all
set GENERATE_QA_REPORT=true
set VALIDATE_DEPLOYMENT=true
set CHECK_DEPENDENCIES=true
set PERFORMANCE_VALIDATION=false

echo Validation Configuration:
echo - Validation Mode: %VALIDATION_MODE%
echo - Target Component: %TARGET_COMPONENT%
echo - Generate QA Report: %GENERATE_QA_REPORT%
echo - Validate Deployment: %VALIDATE_DEPLOYMENT%
echo - Check Dependencies: %CHECK_DEPENDENCIES%
echo - Performance Validation: %PERFORMANCE_VALIDATION%
echo.

REM Optional: Customize validation
echo Customize validation? (y/N)
set /p CUSTOMIZE="Enter choice: "

if /i "%CUSTOMIZE%"=="y" (
    echo.
    echo Validation Mode Options:
    echo 1. comprehensive - Full validation of all aspects
    echo 2. quick - Fast validation focusing on critical issues
    echo 3. specific - Target specific component
    echo.
    set /p VALIDATION_MODE="Enter validation mode: "
    
    if /i "%VALIDATION_MODE%"=="specific" (
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
    set /p VALIDATE_DEPLOYMENT="Validate deployment? (Y/n): "
    if /i "%VALIDATE_DEPLOYMENT%"=="n" set VALIDATE_DEPLOYMENT=false
    
    echo.
    set /p CHECK_DEPENDENCIES="Check dependencies? (Y/n): "
    if /i "%CHECK_DEPENDENCIES%"=="n" set CHECK_DEPENDENCIES=false
    
    echo.
    set /p PERFORMANCE_VALIDATION="Enable performance validation? (y/N): "
    if /i "%PERFORMANCE_VALIDATION%"=="y" set PERFORMANCE_VALIDATION=true
)

echo.
echo Starting build validation...
echo.

REM Run PowerShell validation script
%PWSH_CMD% -ExecutionPolicy Bypass -File "%~dp0build_validator.ps1" -ValidationMode %VALIDATION_MODE% -TargetComponent %TARGET_COMPONENT% -GenerateQAReport:%GENERATE_QA_REPORT% -ValidateDeployment:%VALIDATE_DEPLOYMENT% -CheckDependencies:%CHECK_DEPENDENCIES% -PerformanceValidation:%PERFORMANCE_VALIDATION%

REM Check exit code
if %errorlevel% neq 0 (
    echo.
    echo ========================================
    echo VALIDATION FAILED
    echo ========================================
    echo Build validation failed
    echo Check the logs in logs/validation/ for details
    echo.
    pause
    exit /b 1
) else (
    echo.
    echo ========================================
    echo VALIDATION COMPLETED
    echo ========================================
    echo Build validation completed successfully
    echo Reports saved to: logs/validation/
    echo.
    echo Review the validation results for:
    echo - Build process validation
    echo - Deployment verification
    echo - Dependency checks
    echo - QA validation status
    echo.
)

pause
