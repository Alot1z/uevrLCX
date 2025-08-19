@echo off
REM Cyberpunk 2077 REDengine 4 Adapter - Static Analysis Script
REM Performs comprehensive static analysis on source code
REM This script identifies potential code quality issues

echo ========================================
echo REDengine 4 Adapter Static Analysis
echo ========================================
echo.

set ANALYSIS_DIR=%~dp0
set SOURCE_DIR=%ANALYSIS_DIR%
set REPORT_FILE=%ANALYSIS_DIR%static_analysis_report.txt

echo Static Analysis Report > "%REPORT_FILE%"
echo Generated: %date% %time% >> "%REPORT_FILE%"
echo ======================================== >> "%REPORT_FILE%"
echo. >> "%REPORT_FILE%"

echo [1/6] Analyzing source files...
echo.

REM Check file structure
echo File Structure Analysis >> "%REPORT_FILE%"
echo ---------------------- >> "%REPORT_FILE%"
if exist "%SOURCE_DIR%redengine_adapter.cpp" (
    echo [OK] redengine_adapter.cpp found
    echo [OK] redengine_adapter.cpp found >> "%REPORT_FILE%"
) else (
    echo [ERROR] redengine_adapter.cpp missing
    echo [ERROR] redengine_adapter.cpp missing >> "%REPORT_FILE%"
)

if exist "%SOURCE_DIR%redengine_adapter.hpp" (
    echo [OK] redengine_adapter.hpp found
    echo [OK] redengine_adapter.hpp found >> "%REPORT_FILE%"
) else (
    echo [ERROR] redengine_adapter.hpp missing
    echo [ERROR] redengine_adapter.hpp missing >> "%REPORT_FILE%"
)

if exist "%SOURCE_DIR%uobject_hook.cpp" (
    echo [OK] uobject_hook.cpp found
    echo [OK] uobject_hook.cpp found >> "%REPORT_FILE%"
) else (
    echo [ERROR] uobject_hook.cpp missing
    echo [ERROR] uobject_hook.cpp missing >> "%REPORT_FILE%"
)

if exist "%SOURCE_DIR%uobject_hook.hpp" (
    echo [OK] uobject_hook.hpp found
    echo [OK] uobject_hook.hpp found >> "%REPORT_FILE%"
) else (
    echo [ERROR] uobject_hook.hpp missing
    echo [ERROR] uobject_hook.hpp missing >> "%REPORT_FILE%"
)

if exist "%SOURCE_DIR%CMakeLists.txt" (
    echo [OK] CMakeLists.txt found
    echo [OK] CMakeLists.txt found >> "%REPORT_FILE%"
) else (
    echo [ERROR] CMakeLists.txt missing
    echo [ERROR] CMakeLists.txt missing >> "%REPORT_FILE%"
)

echo. >> "%REPORT_FILE%"

echo [2/6] Checking for TODO/FIXME comments...
echo TODO/FIXME Analysis >> "%REPORT_FILE%"
echo ------------------- >> "%REPORT_FILE%"

REM Check for TODO comments
echo - Scanning for TODO comments...
findstr /s /i /n "TODO" "%SOURCE_DIR%*.cpp" "%SOURCE_DIR%*.hpp" > temp_todo.txt
if %errorlevel% equ 0 (
    echo [WARNING] TODO comments found:
    type temp_todo.txt
    echo [WARNING] TODO comments found: >> "%REPORT_FILE%"
    type temp_todo.txt >> "%REPORT_FILE%"
    echo. >> "%REPORT_FILE%"
) else (
    echo [OK] No TODO comments found
    echo [OK] No TODO comments found >> "%REPORT_FILE%"
)

REM Check for FIXME comments
echo - Scanning for FIXME comments...
findstr /s /i /n "FIXME" "%SOURCE_DIR%*.cpp" "%SOURCE_DIR%*.hpp" > temp_fixme.txt
if %errorlevel% equ 0 (
    echo [WARNING] FIXME comments found:
    type temp_fixme.txt
    echo [WARNING] FIXME comments found: >> "%REPORT_FILE%"
    type temp_fixme.txt >> "%REPORT_FILE%"
    echo. >> "%REPORT_FILE%"
) else (
    echo [OK] No FIXME comments found
    echo [OK] No FIXME comments found >> "%REPORT_FILE%"
)

echo. >> "%REPORT_FILE%"

echo [3/6] Checking for hardcoded values...
echo Hardcoded Values Analysis >> "%REPORT_FILE%"
echo ------------------------- >> "%REPORT_FILE%"

REM Check for hardcoded memory addresses
echo - Scanning for hardcoded memory addresses...
findstr /s /n "0x[0-9A-Fa-f]\{8,16\}" "%SOURCE_DIR%*.cpp" "%SOURCE_DIR%*.hpp" > temp_addresses.txt
if %errorlevel% equ 0 (
    echo [INFO] Hardcoded addresses found:
    type temp_addresses.txt
    echo [INFO] Hardcoded addresses found: >> "%REPORT_FILE%"
    type temp_addresses.txt >> "%REPORT_FILE%"
    echo. >> "%REPORT_FILE%"
) else (
    echo [OK] No hardcoded addresses found
    echo [OK] No hardcoded addresses found >> "%REPORT_FILE%"
)

REM Check for magic numbers
echo - Scanning for magic numbers...
findstr /s /n "[^a-zA-Z_][0-9]\{3,\}[^a-zA-Z_]" "%SOURCE_DIR%*.cpp" "%SOURCE_DIR%*.hpp" > temp_magic.txt
if %errorlevel% equ 0 (
    echo [INFO] Potential magic numbers found:
    type temp_magic.txt
    echo [INFO] Potential magic numbers found: >> "%REPORT_FILE%"
    type temp_magic.txt >> "%REPORT_FILE%"
    echo. >> "%REPORT_FILE%"
) else (
    echo [OK] No obvious magic numbers found
    echo [OK] No obvious magic numbers found >> "%REPORT_FILE%"
)

echo. >> "%REPORT_FILE%"

echo [4/6] Checking for potential memory leaks...
echo Memory Management Analysis >> "%REPORT_FILE%"
echo -------------------------- >> "%REPORT_FILE%"

REM Check for new without delete
echo - Scanning for new without delete...
findstr /s /i /n "new " "%SOURCE_DIR%*.cpp" > temp_new.txt
if %errorlevel% equ 0 (
    echo [INFO] new operators found:
    type temp_new.txt
    echo [INFO] new operators found: >> "%REPORT_FILE%"
    type temp_new.txt >> "%REPORT_FILE%"
    echo. >> "%REPORT_FILE%"
) else (
    echo [OK] No new operators found
    echo [OK] No new operators found >> "%REPORT_FILE%"
)

REM Check for delete without new
findstr /s /i /n "delete " "%SOURCE_DIR%*.cpp" > temp_delete.txt
if %errorlevel% equ 0 (
    echo [INFO] delete operators found:
    type temp_delete.txt
    echo [INFO] delete operators found: >> "%REPORT_FILE%"
    type temp_delete.txt >> "%REPORT_FILE%"
    echo. >> "%REPORT_FILE%"
) else (
    echo [OK] No delete operators found
    echo [OK] No delete operators found >> "%REPORT_FILE%"
)

echo. >> "%REPORT_FILE%"

echo [5/6] Checking for potential null pointer issues...
echo Null Pointer Analysis >> "%REPORT_FILE%"
echo --------------------- >> "%REPORT_FILE%"

REM Check for potential null pointer dereferences
echo - Scanning for potential null pointer issues...
findstr /s /i /n "->" "%SOURCE_DIR%*.cpp" > temp_arrow.txt
if %errorlevel% equ 0 (
    echo [INFO] Arrow operators found (check for null checks):
    type temp_arrow.txt
    echo [INFO] Arrow operators found (check for null checks): >> "%REPORT_FILE%"
    type temp_arrow.txt >> "%REPORT_FILE%"
    echo. >> "%REPORT_FILE%"
) else (
    echo [OK] No arrow operators found
    echo [OK] No arrow operators found >> "%REPORT_FILE%"
)

echo. >> "%REPORT_FILE%"

echo [6/6] Checking for proper error handling...
echo Error Handling Analysis >> "%REPORT_FILE%"
echo ----------------------- >> "%REPORT_FILE%"

REM Check for error handling patterns
echo - Scanning for error handling...
findstr /s /i /n "if.*error\|if.*fail\|if.*null\|if.*invalid" "%SOURCE_DIR%*.cpp" > temp_error_handling.txt
if %errorlevel% equ 0 (
    echo [INFO] Error handling patterns found:
    type temp_error_handling.txt
    echo [INFO] Error handling patterns found: >> "%REPORT_FILE%"
    type temp_error_handling.txt >> "%REPORT_FILE%"
    echo. >> "%REPORT_FILE%"
) else (
    echo [WARNING] Limited error handling patterns found
    echo [WARNING] Limited error handling patterns found >> "%REPORT_FILE%"
)

echo. >> "%REPORT_FILE%"

REM Cleanup temporary files
del temp_*.txt >nul 2>nul

echo ========================================
echo Static Analysis Complete
echo ========================================
echo.
echo Report saved to: %REPORT_FILE%
echo.
echo Summary:
echo - File structure: Verified
echo - TODO/FIXME: Checked
echo - Hardcoded values: Identified
echo - Memory management: Analyzed
echo - Null pointer safety: Checked
echo - Error handling: Reviewed
echo.
echo Review the report for any issues that need attention.
echo.
pause
