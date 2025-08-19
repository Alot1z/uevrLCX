@echo off
REM Master Validation Executor for uevr Cross-Engine VR Adapters
REM Executes validation status finders for all engines and provides comprehensive overview
REM This script validates the complete cross-engine VR adapter system

echo ========================================
echo uevr CROSS-ENGINE VALIDATION EXECUTOR
echo ========================================
echo.
echo This script will execute validation status finders for:
echo 1. Monster Hunter World (MT Framework)
echo 2. Cyberpunk 2077 (REDengine 4)  
echo 3. Resident Evil 7 (RE Engine)
echo 4. Main Cross-Engine Framework
echo.
echo ========================================
echo.

REM Set error handling
setlocal enabledelayedexpansion

REM Create validation results directory
if not exist "validation_results" mkdir "validation_results"
set RESULTS_DIR=validation_results

echo [1/5] Executing Monster Hunter World (MT Framework) Validation...
echo ========================================
cd adapters\MT-Framework
if exist "mhw_validation_status_finder.cpp" (
    echo - Compiling MHW validation finder...
    g++ -std=c++17 -o mhw_validator.exe mhw_validation_status_finder.cpp
    if exist "mhw_validator.exe" (
        echo - Executing MHW validation...
        mhw_validator.exe > ..\..\%RESULTS_DIR%\mhw_validation_output.txt 2>&1
        echo - MHW validation completed
    ) else (
        echo - ERROR: Failed to compile MHW validation finder
    )
) else (
    echo - ERROR: MHW validation finder not found
)
cd ..\..

echo.
echo [2/5] Executing Cyberpunk 2077 (REDengine 4) Validation...
echo ========================================
cd adapters\REDengine4
if exist "cp2077_validation_status_finder.cpp" (
    echo - Compiling CP2077 validation finder...
    g++ -std=c++17 -o cp2077_validator.exe cp2077_validation_status_finder.cpp
    if exist "cp2077_validator.exe" (
        echo - Executing CP2077 validation...
        cp2077_validator.exe > ..\..\%RESULTS_DIR%\cp2077_validation_output.txt 2>&1
        echo - CP2077 validation completed
    ) else (
        echo - ERROR: Failed to compile CP2077 validation finder
    )
) else (
    echo - ERROR: CP2077 validation finder not found
)
cd ..\..

echo.
echo [3/5] Executing Resident Evil 7 (RE Engine) Validation...
echo ========================================
cd adapters\RE-Engine
if exist "re7_validation_status_finder.cpp" (
    echo - Compiling RE7 validation finder...
    g++ -std=c++17 -o re7_validator.exe re7_validation_status_finder.cpp
    if exist "re7_validator.exe" (
        echo - Executing RE7 validation...
        re7_validator.exe > ..\..\%RESULTS_DIR%\re7_validation_output.txt 2>&1
        echo - RE7 validation completed
    ) else (
        echo - ERROR: Failed to compile RE7 validation finder
    )
) else (
    echo - ERROR: RE7 validation finder not found
)
cd ..\..

echo.
echo [4/5] Executing Main Cross-Engine Framework Validation...
echo ========================================
cd include\uevr
if exist "cross_engine_validation_status_finder.cpp" (
    echo - Compiling framework validation finder...
    g++ -std=c++17 -o framework_validator.exe cross_engine_validation_status_finder.cpp
    if exist "framework_validator.exe" (
        echo - Executing framework validation...
        framework_validator.exe > ..\..\%RESULTS_DIR%\framework_validation_output.txt 2>&1
        echo - Framework validation completed
    ) else (
        echo - ERROR: Failed to compile framework validation finder
    )
) else (
    echo - ERROR: Framework validation finder not found
)
cd ..\..

echo.
echo [5/5] Generating Master Validation Report...
echo ========================================
echo.

REM Generate master validation summary
echo uevr Cross-Engine VR Adapter - Master Validation Report > "%RESULTS_DIR%\master_validation_report.txt"
echo Generated: %date% %time% >> "%RESULTS_DIR%\master_validation_report.txt"
echo ======================================== >> "%RESULTS_DIR%\master_validation_report.txt"
echo. >> "%RESULTS_DIR%\master_validation_report.txt"

echo MASTER VALIDATION SUMMARY >> "%RESULTS_DIR%\master_validation_report.txt"
echo ========================= >> "%RESULTS_DIR%\master_validation_report.txt"
echo. >> "%RESULTS_DIR%\master_validation_report.txt"

REM Check MHW validation results
if exist "%RESULTS_DIR%\mhw_validation_output.txt" (
    echo Monster Hunter World (MT Framework) Validation: >> "%RESULTS_DIR%\master_validation_report.txt"
    echo ------------------------------------------------- >> "%RESULTS_DIR%\master_validation_report.txt"
    findstr /C:"Status:" "%RESULTS_DIR%\mhw_validation_output.txt" >> "%RESULTS_DIR%\master_validation_report.txt"
    findstr /C:"Completion:" "%RESULTS_DIR%\mhw_validation_output.txt" >> "%RESULTS_DIR%\master_validation_report.txt"
    echo. >> "%RESULTS_DIR%\master_validation_report.txt"
) else (
    echo Monster Hunter World (MT Framework) Validation: FAILED >> "%RESULTS_DIR%\master_validation_report.txt"
    echo. >> "%RESULTS_DIR%\master_validation_report.txt"
)

REM Check CP2077 validation results
if exist "%RESULTS_DIR%\cp2077_validation_output.txt" (
    echo Cyberpunk 2077 (REDengine 4) Validation: >> "%RESULTS_DIR%\master_validation_report.txt"
    echo ----------------------------------------- >> "%RESULTS_DIR%\master_validation_report.txt"
    findstr /C:"Status:" "%RESULTS_DIR%\cp2077_validation_output.txt" >> "%RESULTS_DIR%\master_validation_report.txt"
    findstr /C:"Completion:" "%RESULTS_DIR%\cp2077_validation_output.txt" >> "%RESULTS_DIR%\master_validation_report.txt"
    findstr /C:"Total TODOs:" "%RESULTS_DIR%\cp2077_validation_output.txt" >> "%RESULTS_DIR%\master_validation_report.txt"
    echo. >> "%RESULTS_DIR%\master_validation_report.txt"
) else (
    echo Cyberpunk 2077 (REDengine 4) Validation: FAILED >> "%RESULTS_DIR%\master_validation_report.txt"
    echo. >> "%RESULTS_DIR%\master_validation_report.txt"
)

REM Check RE7 validation results
if exist "%RESULTS_DIR%\re7_validation_output.txt" (
    echo Resident Evil 7 (RE Engine) Validation: >> "%RESULTS_DIR%\master_validation_report.txt"
    echo --------------------------------------- >> "%RESULTS_DIR%\master_validation_report.txt"
    findstr /C:"Status:" "%RESULTS_DIR%\re7_validation_output.txt" >> "%RESULTS_DIR%\master_validation_report.txt"
    findstr /C:"Completion:" "%RESULTS_DIR%\re7_validation_output.txt" >> "%RESULTS_DIR%\master_validation_report.txt"
    echo. >> "%RESULTS_DIR%\master_validation_report.txt"
) else (
    echo Resident Evil 7 (RE Engine) Validation: FAILED >> "%RESULTS_DIR%\master_validation_report.txt"
    echo. >> "%RESULTS_DIR%\master_validation_report.txt"
)

REM Check framework validation results
if exist "%RESULTS_DIR%\framework_validation_output.txt" (
    echo Main Cross-Engine Framework Validation: >> "%RESULTS_DIR%\master_validation_report.txt"
    echo -------------------------------------- >> "%RESULTS_DIR%\master_validation_report.txt"
    findstr /C:"Status:" "%RESULTS_DIR%\framework_validation_output.txt" >> "%RESULTS_DIR%\master_validation_report.txt"
    findstr /C:"Completion:" "%RESULTS_DIR%\framework_validation_output.txt" >> "%RESULTS_DIR%\master_validation_report.txt"
    echo. >> "%RESULTS_DIR%\master_validation_report.txt"
) else (
    echo Main Cross-Engine Framework Validation: FAILED >> "%RESULTS_DIR%\master_validation_report.txt"
    echo. >> "%RESULTS_DIR%\master_validation_report.txt"
)

REM Generate overall project status
echo OVERALL PROJECT STATUS >> "%RESULTS_DIR%\master_validation_report.txt"
echo ===================== >> "%RESULTS_DIR%\master_validation_report.txt"
echo. >> "%RESULTS_DIR%\master_validation_report.txt"

REM Count successful validations
set SUCCESS_COUNT=0
if exist "%RESULTS_DIR%\mhw_validation_output.txt" set /a SUCCESS_COUNT+=1
if exist "%RESULTS_DIR%\cp2077_validation_output.txt" set /a SUCCESS_COUNT+=1
if exist "%RESULTS_DIR%\re7_validation_output.txt" set /a SUCCESS_COUNT+=1
if exist "%RESULTS_DIR%\framework_validation_output.txt" set /a SUCCESS_COUNT+=1

echo Total Components Validated: 4 >> "%RESULTS_DIR%\master_validation_report.txt"
echo Successful Validations: %SUCCESS_COUNT% >> "%RESULTS_DIR%\master_validation_report.txt"
echo Failed Validations: %((4-SUCCESS_COUNT))% >> "%RESULTS_DIR%\master_validation_report.txt"
echo. >> "%RESULTS_DIR%\master_validation_report.txt"

if %SUCCESS_COUNT% equ 4 (
    echo Overall Status: ALL COMPONENTS VALIDATED SUCCESSFULLY >> "%RESULTS_DIR%\master_validation_report.txt"
    echo Next Action: Proceed with QA validation for ready components >> "%RESULTS_DIR%\master_validation_report.txt"
) else if %SUCCESS_COUNT% geq 3 (
    echo Overall Status: MOSTLY VALIDATED - Some issues to resolve >> "%RESULTS_DIR%\master_validation_report.txt"
    echo Next Action: Review failed validations and resolve issues >> "%RESULTS_DIR%\master_validation_report.txt"
) else (
    echo Overall Status: MULTIPLE VALIDATION FAILURES - Critical issues >> "%RESULTS_DIR%\master_validation_report.txt"
    echo Next Action: Fix validation system and re-run all validations >> "%RESULTS_DIR%\master_validation_report.txt"
)

echo. >> "%RESULTS_DIR%\master_validation_report.txt"
echo ======================================== >> "%RESULTS_DIR%\master_validation_report.txt"
echo Master validation report saved to: %RESULTS_DIR%\master_validation_report.txt >> "%RESULTS_DIR%\master_validation_report.txt"

echo.
echo ========================================
echo MASTER VALIDATION COMPLETED
echo ========================================
echo.
echo Validation Results:
echo - MHW (MT Framework): %(if exist "%RESULTS_DIR%\mhw_validation_output.txt" echo "COMPLETED" else echo "FAILED")%
echo - CP2077 (REDengine 4): %(if exist "%RESULTS_DIR%\cp2077_validation_output.txt" echo "COMPLETED" else echo "FAILED")%
echo - RE7 (RE Engine): %(if exist "%RESULTS_DIR%\re7_validation_output.txt" echo "COMPLETED" else echo "FAILED")%
echo - Framework: %(if exist "%RESULTS_DIR%\framework_validation_output.txt" echo "COMPLETED" else echo "FAILED")%
echo.
echo Overall Success Rate: %SUCCESS_COUNT% of 4 (%((SUCCESS_COUNT*100/4))%%)
echo.
echo Detailed results saved to: %RESULTS_DIR%\
echo Master report: %RESULTS_DIR%\master_validation_report.txt
echo.
echo ========================================
echo VALIDATION EXECUTION COMPLETE
echo ========================================
echo.
pause
