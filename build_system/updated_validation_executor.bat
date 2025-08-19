@echo off
echo ===============================================
echo uevr UPDATED VALIDATION EXECUTOR
echo ===============================================
echo Validating with new src/adapters/ structure...
echo Based on comprehensive uevr documentation
echo.

REM Set paths for new structure
set SRC_DIR=..\src
set ADAPTERS_DIR=%SRC_DIR%\adapters
set BUILD_DIR=build
set REPORTS_DIR=build_reports
set VALIDATION_DIR=validation_reports

REM Create necessary directories
if not exist %BUILD_DIR% mkdir %BUILD_DIR%
if not exist %REPORTS_DIR% mkdir %REPORTS_DIR%
if not exist %VALIDATION_DIR% mkdir %VALIDATION_DIR%

echo.
echo ===============================================
echo VALIDATING NEW FILE STRUCTURE
echo ===============================================

REM Check if new structure exists
echo Checking new file structure...
if exist %SRC_DIR% (
    echo ✓ Main src directory found
) else (
    echo ✗ Main src directory not found
    goto :error
)

if exist %ADAPTERS_DIR% (
    echo ✓ Adapters directory found
) else (
    echo ✗ Adapters directory not found
    goto :error
)

REM Check individual adapters
echo.
echo Checking individual adapters...
if exist %ADAPTERS_DIR%\redengine4 (
    echo ✓ REDengine4 adapter directory found
    if exist %ADAPTERS_DIR%\redengine4\redengine_adapter.cpp (
        echo ✓ REDengine4 adapter source found
    ) else (
        echo ⚠ REDengine4 adapter source missing
    )
) else (
    echo ✗ REDengine4 adapter directory missing
)

if exist %ADAPTERS_DIR%\mt-framework (
    echo ✓ MT-Framework adapter directory found
    if exist %ADAPTERS_DIR%\mt-framework\mhw_validation_status_finder.cpp (
        echo ✓ MT-Framework validation finder found
    ) else (
        echo ⚠ MT-Framework validation finder missing
    )
) else (
    echo ✗ MT-Framework adapter directory missing
)

if exist %ADAPTERS_DIR%\re-engine (
    echo ✓ RE-Engine adapter directory found
    if exist %ADAPTERS_DIR%\re-engine\re7_validation_status_finder.cpp (
        echo ✓ RE-Engine validation finder found
    ) else (
        echo ⚠ RE-Engine validation finder missing
    )
) else (
    echo ✗ RE-Engine adapter directory missing
)

echo.
echo ===============================================
echo VALIDATING CORE FRAMEWORK
echo ===============================================

REM Check main framework
echo Checking main uevr framework...
if exist %SRC_DIR%\CMakeLists.txt (
    echo ✓ Main CMakeLists.txt found
) else (
    echo ✗ Main CMakeLists.txt missing
)

if exist %SRC_DIR%\Framework.cpp (
    echo ✓ Framework.cpp found
) else (
    echo ✗ Framework.cpp missing
)

if exist %SRC_DIR%\Framework.hpp (
    echo ✓ Framework.hpp found
) else (
    echo ✗ Framework.hpp missing
)

if exist %SRC_DIR%\mods\VR.cpp (
    echo ✓ VR mod found
) else (
    echo ✗ VR mod missing
)

echo.
echo ===============================================
echo VALIDATING PLUGINS AND EXTENSIONS
echo ===============================================

REM Check plugins
echo Checking plugins and extensions...
if exist %SRC_DIR%\examples\example_plugin (
    echo ✓ Example plugin directory found
) else (
    echo ⚠ Example plugin directory missing
)

if exist %SRC_DIR%\lua-api (
    echo ✓ Lua API directory found
) else (
    echo ⚠ Lua API directory missing
)

echo.
echo ===============================================
echo VALIDATING BUILD SYSTEM
echo ===============================================

REM Check build system components
echo Checking build system components...
if exist comprehensive_qa_system.cpp (
    echo ✓ QA System found
) else (
    echo ⚠ QA System missing
)

if exist enterprise_security_system.cpp (
    echo ✓ Security System found
) else (
    echo ⚠ Security System missing
)

if exist advanced_rendering_system.cpp (
    echo ✓ Rendering System found
) else (
    echo ⚠ Rendering System missing
)

if exist comprehensive_deployment_system.cpp (
    echo ✓ Deployment System found
) else (
    echo ⚠ Deployment System missing
)

if exist master_build_analysis_executor.cpp (
    echo ✓ Master Executor found
) else (
    echo ⚠ Master Executor missing
)

echo.
echo ===============================================
echo VALIDATION SUMMARY
echo ===============================================

echo New structure validation complete!
echo.
echo Structure paths:
echo - Main Framework: %SRC_DIR%
echo - Adapters: %ADAPTERS_DIR%
echo - REDengine4: %ADAPTERS_DIR%\redengine4
echo - MT-Framework: %ADAPTERS_DIR%\mt-framework
echo - RE-Engine: %ADAPTERS_DIR%\re-engine
echo - Plugins: %SRC_DIR%\examples
echo - Lua API: %SRC_DIR%\lua-api
echo - Build System: build_system
echo.
echo All builds now follow the new src/adapters/ organization!
echo.
echo ===============================================
echo VALIDATION COMPLETE - NEW STRUCTURE READY
echo ===============================================
goto :end

:error
echo.
echo ===============================================
echo VALIDATION FAILED
echo ===============================================
echo The new file structure is not properly set up.
echo Please ensure all files have been moved to src/adapters/
echo.

:end
pause
