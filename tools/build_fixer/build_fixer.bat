@echo off
REM uevr Cross-Engine VR Adapters - Build Fixer (Batch Wrapper)
REM Automatically fixes common build issues and errors
REM Copyright (c) 2024 uevr Cross-Engine Adapter Project

echo ========================================
echo uevr Build Fixer
echo ========================================
echo.
echo This tool automatically fixes common build issues:
echo - Compilation errors and warnings
echo - Missing dependencies
echo - Configuration problems
echo - Build script errors
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

REM Set fixer parameters
set FIX_MODE=comprehensive
set TARGET_COMPONENT=all
set AUTO_FIX=true
set BACKUP_ORIGINALS=true
set VALIDATE_FIXES=true
set GENERATE_REPORT=true

echo Fixer Configuration:
echo - Fix Mode: %FIX_MODE%
echo - Target Component: %TARGET_COMPONENT%
echo - Auto-Fix Issues: %AUTO_FIX%
echo - Backup Originals: %BACKUP_ORIGINALS%
echo - Validate Fixes: %VALIDATE_FIXES%
echo - Generate Report: %GENERATE_REPORT%
echo.

REM Optional: Customize fixer
echo Customize fixer? (y/N)
set /p CUSTOMIZE="Enter choice: "

if /i "%CUSTOMIZE%"=="y" (
    echo.
    echo Fix Mode Options:
    echo 1. comprehensive - Fix all types of issues
    echo 2. critical - Fix only critical errors
    echo 3. warnings - Fix warnings and non-critical issues
    echo 4. specific - Target specific component
    echo 5. safe - Fix only safe, well-tested issues
    echo.
    set /p FIX_MODE="Enter fix mode: "
    
    if /i "%FIX_MODE%"=="specific" (
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
    set /p AUTO_FIX="Enable automatic fixing? (Y/n): "
    if /i "%AUTO_FIX%"=="n" set AUTO_FIX=false
    
    echo.
    set /p BACKUP_ORIGINALS="Backup original files before fixing? (Y/n): "
    if /i "%BACKUP_ORIGINALS%"=="n" set BACKUP_ORIGINALS=false
    
    echo.
    set /p VALIDATE_FIXES="Validate fixes after applying? (Y/n): "
    if /i "%VALIDATE_FIXES%"=="n" set VALIDATE_FIXES=false
)

echo.
echo Starting build fixing...
echo.

REM Run PowerShell fixer script
%PWSH_CMD% -ExecutionPolicy Bypass -File "%~dp0build_fixer.ps1" -FixMode %FIX_MODE% -TargetComponent %TARGET_COMPONENT% -AutoFix:%AUTO_FIX% -BackupOriginals:%BACKUP_ORIGINALS% -ValidateFixes:%VALIDATE_FIXES% -GenerateReport:%GENERATE_REPORT%

REM Check exit code
if %errorlevel% neq 0 (
    echo.
    echo ========================================
    echo FIXING FAILED
    echo ========================================
    echo Build fixing failed
    echo Check the logs in logs/fixing/ for details
    echo.
    pause
    exit /b 1
) else (
    echo.
    echo ========================================
    echo FIXING COMPLETED
    echo ========================================
    echo Build fixing completed successfully
    echo Reports saved to: logs/fixing/
    echo.
    echo Review the fixing results for:
    echo - Issues identified and fixed
    echo - Fixes applied successfully
    echo - Validation results
    echo - Remaining issues (if any)
    echo.
)

pause
