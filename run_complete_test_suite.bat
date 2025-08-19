@echo off
setlocal enabledelayedexpansion

:: ========================================
:: uevr Complete Test Suite Runner
:: ========================================
:: This script runs all tests for the uevr system including:
:: - Component tests
:: - Integration tests
:: - Performance tests
:: - System validation

:: Set test configuration
set TEST_MODE=full
set PERFORMANCE_TEST=true
set GENERATE_REPORT=true
set OPEN_REPORT=false
set CLEAN_RESULTS=false

:: Parse command line arguments
for %%a in (%*) do (
    if "%%a"=="--quick" set TEST_MODE=quick
    if "%%a"=="--full" set TEST_MODE=full
    if "%%a"=="--component" set TEST_MODE=component
    if "%%a"=="--integration" set TEST_MODE=integration
    if "%%a"=="--performance" set TEST_MODE=performance
    if "%%a"=="--noperformance" set PERFORMANCE_TEST=false
    if "%%a"=="--noreport" set GENERATE_REPORT=false
    if "%%a"=="--openreport" set OPEN_REPORT=true
    if "%%a"=="--clean" set CLEAN_RESULTS=true
    if "%%a"=="--help" goto :show_help
)

:: Set test directories
set TEST_RESULTS_DIR=test_results
set TEST_REPORT_DIR=test_reports
set BUILD_DIR=build_x64_Release
set INSTALL_DIR=install_x64_Release

:: Show test configuration
echo ========================================
echo uevr Complete Test Suite Configuration
echo ========================================
echo Test Mode: %TEST_MODE%
echo Performance Testing: %PERFORMANCE_TEST%
echo Generate Report: %GENERATE_REPORT%
echo Open Report: %OPEN_REPORT%
echo Clean Results: %CLEAN_RESULTS%
echo Test Results Directory: %TEST_RESULTS_DIR%
echo Test Report Directory: %TEST_REPORT_DIR%
echo Build Directory: %BUILD_DIR%
echo Install Directory: %INSTALL_DIR%
echo ========================================
echo.

:: Check prerequisites
echo Checking prerequisites...
if not exist "test_full_uevr_system.ps1" (
    echo ERROR: Test suite not found!
    echo Please ensure test_full_uevr_system.ps1 exists.
    pause
    exit /b 1
)

if not exist "%BUILD_DIR%" (
    echo WARNING: Build directory not found: %BUILD_DIR%
    echo Some tests may fail. Consider building first.
    echo.
)

if not exist "%INSTALL_DIR%" (
    echo WARNING: Install directory not found: %INSTALL_DIR%
    echo Some tests may fail. Consider building first.
    echo.
)

:: Clean test results if requested
if "%CLEAN_RESULTS%"=="true" (
    echo Cleaning test results...
    if exist "%TEST_RESULTS_DIR%" rmdir /s /q "%TEST_RESULTS_DIR%"
    if exist "%TEST_REPORT_DIR%" rmdir /s /q "%TEST_REPORT_DIR%"
    echo Test results cleaned.
    echo.
)

:: Create test directories
if not exist "%TEST_RESULTS_DIR%" mkdir "%TEST_RESULTS_DIR%"
if not exist "%TEST_REPORT_DIR%" mkdir "%TEST_REPORT_DIR%"

:: Set timestamp for this test run
for /f "tokens=2 delims==" %%a in ('wmic OS Get localdatetime /value') do set "dt=%%a"
set "YYYY=%dt:~2,2%"
set "MM=%dt:~4,2%"
set "DD=%dt:~6,2%"
set "HH=%dt:~8,2%"
set "Min=%dt:~10,2%"
set "Sec=%dt:~12,2%"
set TIMESTAMP=%YYYY%%MM%%DD%_%HH%%Min%%Sec%

:: Run tests based on mode
echo Starting test execution...
echo Test run timestamp: %TIMESTAMP%
echo.

if "%TEST_MODE%"=="quick" (
    echo Running QUICK test suite...
    echo.
    echo 1. Component validation...
    powershell -ExecutionPolicy Bypass -File "test_full_uevr_system.ps1" -SkipBuild -FullSystem
    echo.
    echo 2. Basic integration test...
    powershell -ExecutionPolicy Bypass -File "test_uevr_system.ps1" -SkipBuild
    echo.
    echo Quick test suite completed!
    
) else if "%TEST_MODE%"=="component" (
    echo Running COMPONENT test suite...
    echo.
    powershell -ExecutionPolicy Bypass -File "test_full_uevr_system.ps1" -SkipBuild -FullSystem
    echo.
    echo Component test suite completed!
    
) else if "%TEST_MODE%"=="integration" (
    echo Running INTEGRATION test suite...
    echo.
    powershell -ExecutionPolicy Bypass -File "test_uevr_system.ps1" -SkipBuild
    echo.
    echo Integration test suite completed!
    
) else if "%TEST_MODE%"=="performance" (
    echo Running PERFORMANCE test suite...
    echo.
    powershell -ExecutionPolicy Bypass -File "test_full_uevr_system.ps1" -SkipBuild -FullSystem -PerformanceTest
    echo.
    echo Performance test suite completed!
    
) else (
    echo Running FULL test suite...
    echo.
    echo 1. Complete system test...
    powershell -ExecutionPolicy Bypass -File "test_full_uevr_system.ps1" -FullSystem -PerformanceTest
    echo.
    echo 2. Basic system test...
    powershell -ExecutionPolicy Bypass -File "test_uevr_system.ps1"
    echo.
    echo Full test suite completed!
)

:: Generate comprehensive test report
if "%GENERATE_REPORT%"=="true" (
    echo.
    echo Generating comprehensive test report...
    
    set REPORT_FILE=%TEST_REPORT_DIR%\uevr_test_report_%TIMESTAMP%.md
    
    echo # uevr Complete Test Suite Report > "%REPORT_FILE%"
    echo. >> "%REPORT_FILE%"
    echo **Generated:** %date% %time% >> "%REPORT_FILE%"
    echo **Test Mode:** %TEST_MODE% >> "%REPORT_FILE%"
    echo **Performance Testing:** %PERFORMANCE_TEST% >> "%REPORT_FILE%"
    echo **Timestamp:** %TIMESTAMP% >> "%REPORT_FILE%"
    echo. >> "%REPORT_FILE%"
    echo ## Test Summary >> "%REPORT_FILE%"
    echo. >> "%REPORT_FILE%"
    echo ### Test Execution >> "%REPORT_FILE%"
    echo - **Test Mode**: %TEST_MODE% >> "%REPORT_FILE%"
    echo - **Performance Testing**: %PERFORMANCE_TEST% >> "%REPORT_FILE%"
    echo - **Test Results Directory**: %TEST_RESULTS_DIR% >> "%REPORT_FILE%"
    echo - **Build Directory**: %BUILD_DIR% >> "%REPORT_FILE%"
    echo - **Install Directory**: %INSTALL_DIR% >> "%REPORT_FILE%"
    echo. >> "%REPORT_FILE%"
    echo ### System Status >> "%REPORT_FILE%"
    
    :: Check system components
    echo Checking system components...
    echo - **Build System**: >> "%REPORT_FILE%"
    if exist "%BUILD_DIR%" (
        echo   - ✅ Build directory exists >> "%REPORT_FILE%"
        echo   - ✅ Build system available >> "%REPORT_FILE%"
    ) else (
        echo   - ❌ Build directory missing >> "%REPORT_FILE%"
        echo   - ❌ Build system not available >> "%REPORT_FILE%"
    )
    
    echo - **Installation**: >> "%REPORT_FILE%"
    if exist "%INSTALL_DIR%" (
        echo   - ✅ Install directory exists >> "%REPORT_FILE%"
        if exist "%INSTALL_DIR%\bin" (
            echo   - ✅ Binary files available >> "%REPORT_FILE%"
        ) else (
            echo   - ❌ Binary files missing >> "%REPORT_FILE%"
        )
        if exist "%INSTALL_DIR%\config" (
            echo   - ✅ Configuration files available >> "%REPORT_FILE%"
        ) else (
            echo   - ❌ Configuration files missing >> "%REPORT_FILE%"
        )
    ) else (
        echo   - ❌ Install directory missing >> "%REPORT_FILE%"
        echo   - ❌ Installation not available >> "%REPORT_FILE%"
    )
    
    echo - **Test Suite**: >> "%REPORT_FILE%"
    if exist "test_full_uevr_system.ps1" (
        echo   - ✅ Full test suite available >> "%REPORT_FILE%"
    ) else (
        echo   - ❌ Full test suite missing >> "%REPORT_FILE%"
    )
    if exist "test_uevr_system.ps1" (
        echo   - ✅ Basic test suite available >> "%REPORT_FILE%"
    ) else (
        echo   - ❌ Basic test suite missing >> "%REPORT_FILE%"
    )
    
    echo - **Source Code**: >> "%REPORT_FILE%"
    if exist "src\adapters\common\CrossEngineAdapterFactory.hpp" (
        echo   - ✅ Cross-Engine Factory available >> "%REPORT_FILE%"
    ) else (
        echo   - ❌ Cross-Engine Factory missing >> "%REPORT_FILE%"
    )
    if exist "src\adapters\re-engine\REEngineAdapter.hpp" (
        echo   - ✅ RE Engine Adapter available >> "%REPORT_FILE%"
    ) else (
        echo   - ❌ RE Engine Adapter missing >> "%REPORT_FILE%"
    )
    if exist "src\adapters\common\UObjectHookSystem.hpp" (
        echo   - ✅ UObjectHook System available >> "%REPORT_FILE%"
    ) else (
        echo   - ❌ UObjectHook System missing >> "%REPORT_FILE%"
    )
    if exist "include\uevr\hooks\VulkanHooks.hpp" (
        echo   - ✅ Vulkan Hooks available >> "%REPORT_FILE%"
    ) else (
        echo   - ❌ Vulkan Hooks missing >> "%REPORT_FILE%"
    )
    if exist "src\hooks\VulkanHooks.cpp" (
        echo   - ✅ Vulkan Hooks implementation available >> "%REPORT_FILE%"
    ) else (
        echo   - ❌ Vulkan Hooks implementation missing >> "%REPORT_FILE%"
    )
    if exist "include\uevr\vr\OpenXRIntegration.hpp" (
        echo   - ✅ OpenXR Integration available >> "%REPORT_FILE%"
    ) else (
        echo   - ❌ OpenXR Integration missing >> "%REPORT_FILE%"
    )
    if exist "src\vr\OpenXRIntegration.cpp" (
        echo   - ✅ OpenXR Integration implementation available >> "%REPORT_FILE%"
    ) else (
        echo   - ❌ OpenXR Integration implementation missing >> "%REPORT_FILE%"
    )
    
    echo. >> "%REPORT_FILE%"
    echo ### Test Results >> "%REPORT_FILE%"
    echo - **Test Results Directory**: %TEST_RESULTS_DIR% >> "%REPORT_FILE%"
    echo - **Test Report**: %REPORT_FILE% >> "%REPORT_FILE%"
    echo. >> "%REPORT_FILE%"
    echo ### Recommendations >> "%REPORT_FILE%"
    
    :: Generate recommendations based on system state
    if not exist "%BUILD_DIR%" (
        echo - 🔧 **Build the system first** using `build_complete_system.bat` >> "%REPORT_FILE%"
    )
    if not exist "%INSTALL_DIR%" (
        echo - 🔧 **Install the system** after building >> "%REPORT_FILE%"
    )
    if exist "%BUILD_DIR%" if exist "%INSTALL_DIR%" (
        echo - ✅ **System is ready** for testing and use >> "%REPORT_FILE%"
        echo - 🚀 **Next steps**: >> "%REPORT_FILE%"
        echo   - Run performance tests >> "%REPORT_FILE%"
        echo   - Test VR integration >> "%REPORT_FILE%"
        echo   - Validate game compatibility >> "%REPORT_FILE%"
    )
    
    echo. >> "%REPORT_FILE%"
    echo --- >> "%REPORT_FILE%"
    echo *Report generated by uevr Complete Test Suite Runner* >> "%REPORT_FILE%"
    
    echo Test report generated: %REPORT_FILE%
    
    :: Open report if requested
    if "%OPEN_REPORT%"=="true" (
        echo Opening test report...
        start "" "%REPORT_FILE%"
    )
)

:: Test execution summary
echo.
echo ========================================
echo TEST EXECUTION COMPLETED!
echo ========================================
echo.
echo Test Summary:
echo - Test Mode: %TEST_MODE%
echo - Performance Testing: %PERFORMANCE_TEST%
echo - Report Generated: %GENERATE_REPORT%
echo.
echo Test Results:
echo - Results Directory: %TEST_RESULTS_DIR%
if "%GENERATE_REPORT%"=="true" echo - Report File: %REPORT_FILE%
echo.
echo Next Steps:
echo 1. Review test results in %TEST_RESULTS_DIR%
if "%GENERATE_REPORT%"=="true" echo 2. Review test report: %REPORT_FILE%
echo 3. Fix any issues identified
echo 4. Re-run tests to validate fixes
echo 5. Build and install the system
echo.
echo ========================================

:: Prompt to open test results
set /p OPEN_RESULTS="Open test results directory? (y/n): "
if /i "%OPEN_RESULTS%"=="y" (
    explorer "%TEST_RESULTS_DIR%"
)

pause
exit /b 0

:show_help
echo uevr Complete Test Suite Runner
echo.
echo Usage: run_complete_test_suite.bat [options]
echo.
echo Options:
echo   --quick        Quick test (component + basic integration)
echo   --full         Full test suite (default)
echo   --component    Component tests only
echo   --integration  Integration tests only
echo   --performance  Performance tests only
echo   --noperformance Disable performance testing
echo   --noreport     Don't generate test report
echo   --openreport   Open test report after generation
echo   --clean        Clean test results before running
echo   --help         Show this help message
echo.
echo Examples:
echo   run_complete_test_suite.bat --quick
echo   run_complete_test_suite.bat --full --clean
echo   run_complete_test_suite.bat --performance --openreport
echo.
pause
exit /b 0
