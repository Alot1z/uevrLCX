@echo off
setlocal enabledelayedexpansion

:: ========================================
:: UEVR Complete System Build Script
:: ========================================
:: Advanced build system for the complete UEVR project
:: Supports all adapters, Vulkan, OpenXR, and testing

:: Set build configuration
set BUILD_TYPE=Release
set BUILD_ARCH=x64
set BUILD_PLATFORM=Win64
set ENABLE_VR=true
set ENABLE_DEBUG=false
set ENABLE_VULKAN=true
set ENABLE_OPENXR=true
set ENABLE_TESTING=true
set CLEAN_BUILD=false
set VERBOSE_BUILD=false
set BUILD_FULL_SYSTEM=true
set INSTALL_AFTER_BUILD=true
set CREATE_LAUNCHERS=true

:: Parse command line arguments
for %%a in (%*) do (
    if "%%a"=="--debug" set ENABLE_DEBUG=true
    if "%%a"=="--release" set BUILD_TYPE=Release
    if "%%a"=="--x86" set BUILD_ARCH=x86
    if "%%a"=="--x64" set BUILD_ARCH=x64
    if "%%a"=="--novr" set ENABLE_VR=false
    if "%%a"=="--novulkan" set ENABLE_VULKAN=false
    if "%%a"=="--noopenxr" set ENABLE_OPENXR=false
    if "%%a"=="--notest" set ENABLE_TESTING=false
    if "%%a"=="--clean" set CLEAN_BUILD=true
    if "%%a"=="--verbose" set VERBOSE_BUILD=true
    if "%%a"=="--minimal" set BUILD_FULL_SYSTEM=false
    if "%%a"=="--noinstall" set INSTALL_AFTER_BUILD=false
    if "%%a"=="--nolaunchers" set CREATE_LAUNCHERS=false
    if "%%a"=="--help" goto :show_help
)

:: Set build directories
set BUILD_DIR=build_%BUILD_ARCH%_%BUILD_TYPE%
set INSTALL_DIR=install_%BUILD_ARCH%_%BUILD_TYPE%
set SOURCE_DIR=%CD%
set EXTERNAL_DIR=%CD%\external

:: Show build configuration
echo.
echo ========================================
echo UEVR Complete System Build Configuration
echo ========================================
echo Build Type: %BUILD_TYPE%
echo Architecture: %BUILD_ARCH%
echo Platform: %BUILD_PLATFORM%
echo VR Support: %ENABLE_VR%
echo Debug Mode: %ENABLE_DEBUG%
echo Vulkan Support: %ENABLE_VULKAN%
echo OpenXR Support: %ENABLE_OPENXR%
echo Testing: %ENABLE_TESTING%
echo Full System: %BUILD_FULL_SYSTEM%
echo Clean Build: %CLEAN_BUILD%
echo Verbose: %VERBOSE_BUILD%
echo Build Directory: %BUILD_DIR%
echo Install Directory: %INSTALL_DIR%
echo ========================================
echo.

:: Check prerequisites
echo Checking system prerequisites...
where cmake >nul 2>&1
if %ERRORLEVEL% neq 0 (
    echo ERROR: CMake not found in PATH
    echo Please install CMake and add it to your PATH
    goto :error_exit
)

where cl >nul 2>&1
if %ERRORLEVEL% neq 0 (
    echo ERROR: Visual Studio compiler not found
    echo Please run from Visual Studio Developer Command Prompt
    goto :error_exit
)

echo ✅ Prerequisites check passed
echo.

:: Clean build directory if requested
if "%CLEAN_BUILD%"=="true" (
    echo Cleaning build directory...
    if exist "%BUILD_DIR%" (
        rmdir /s /q "%BUILD_DIR%"
        echo ✅ Build directory cleaned
    )
    if exist "%INSTALL_DIR%" (
        rmdir /s /q "%INSTALL_DIR%"
        echo ✅ Install directory cleaned
    )
    echo.
)

:: Create build directory
if not exist "%BUILD_DIR%" (
    mkdir "%BUILD_DIR%"
    echo ✅ Created build directory: %BUILD_DIR%
)

:: Navigate to build directory
cd "%BUILD_DIR%"

:: Set CMake configuration
set CMAKE_CONFIG=-DCMAKE_BUILD_TYPE=%BUILD_TYPE%
set CMAKE_CONFIG=%CMAKE_CONFIG% -DCMAKE_INSTALL_PREFIX=%CD%\..\%INSTALL_DIR%
set CMAKE_CONFIG=%CMAKE_CONFIG% -DCMAKE_GENERATOR_PLATFORM=%BUILD_ARCH%

:: Add VR-specific options
if "%ENABLE_VR%"=="true" (
    set CMAKE_CONFIG=%CMAKE_CONFIG% -DENABLE_VR=ON
    if "%ENABLE_OPENXR%"=="true" (
        set CMAKE_CONFIG=%CMAKE_CONFIG% -DENABLE_OPENXR=ON
        set CMAKE_CONFIG=%CMAKE_CONFIG% -DENABLE_OPENXR_LOADER=ON
    )
) else (
    set CMAKE_CONFIG=%CMAKE_CONFIG% -DENABLE_VR=OFF
    set CMAKE_CONFIG=%CMAKE_CONFIG% -DENABLE_OPENXR=OFF
    set CMAKE_CONFIG=%CMAKE_CONFIG% -DENABLE_OPENXR_LOADER=OFF
)

:: Add Vulkan support
if "%ENABLE_VULKAN%"=="true" (
    set CMAKE_CONFIG=%CMAKE_CONFIG% -DENABLE_VULKAN=ON
    set CMAKE_CONFIG=%CMAKE_CONFIG% -DENABLE_VULKAN_HOOKS=ON
) else (
    set CMAKE_CONFIG=%CMAKE_CONFIG% -DENABLE_VULKAN=OFF
)

:: Add testing support
if "%ENABLE_TESTING%"=="true" (
    set CMAKE_CONFIG=%CMAKE_CONFIG% -DENABLE_TESTING=ON
    set CMAKE_CONFIG=%CMAKE_CONFIG% -DBUILD_TESTING=ON
) else (
    set CMAKE_CONFIG=%CMAKE_CONFIG% -DENABLE_TESTING=OFF
    set CMAKE_CONFIG=%CMAKE_CONFIG% -DBUILD_TESTING=OFF
)

:: Add debug options
if "%ENABLE_DEBUG%"=="true" (
    set CMAKE_CONFIG=%CMAKE_CONFIG% -DCMAKE_BUILD_TYPE=Debug
    set CMAKE_CONFIG=%CMAKE_CONFIG% -DENABLE_SANITIZERS=ON
    set CMAKE_CONFIG=%CMAKE_CONFIG% -DENABLE_DEBUG_SYMBOLS=ON
)

:: Add verbose output
if "%VERBOSE_BUILD%"=="true" (
    set CMAKE_CONFIG=%CMAKE_CONFIG% -DCMAKE_VERBOSE_MAKEFILE=ON
)

:: Choose CMakeLists file
if "%BUILD_FULL_SYSTEM%"=="true" (
    set CMAKE_FILE=..\CMakeLists_full_system.txt
    echo Building FULL SYSTEM with all adapters and features
) else (
    set CMAKE_FILE=..\CMakeLists.txt
    echo Building MINIMAL SYSTEM with basic functionality
)

:: Configure project
echo.
echo Configuring project with CMake...
echo Command: cmake %CMAKE_CONFIG% -f %CMAKE_FILE% ..
if "%VERBOSE_BUILD%"=="true" (
    cmake %CMAKE_CONFIG% -f %CMAKE_FILE% ..
) else (
    cmake %CMAKE_CONFIG% -f %CMAKE_FILE% .. >nul 2>&1
)

if %ERRORLEVEL% neq 0 (
    echo ERROR: CMake configuration failed
    cd ..
    goto :error_exit
)
echo ✅ CMake configuration successful
echo.

:: Build project
echo Building project...
if "%VERBOSE_BUILD%"=="true" (
    cmake --build . --config %BUILD_TYPE% --parallel
) else (
    cmake --build . --config %BUILD_TYPE% --parallel >nul 2>&1
)

if %ERRORLEVEL% neq 0 (
    echo ERROR: Build failed
    cd ..
    goto :error_exit
)
echo ✅ Build successful
echo.

:: Install project
if "%INSTALL_AFTER_BUILD%"=="true" (
    echo Installing project...
    if "%VERBOSE_BUILD%"=="true" (
        cmake --install . --config %BUILD_TYPE%
    ) else (
        cmake --install . --config %BUILD_TYPE% >nul 2>&1
    )
    
    if %ERRORLEVEL% neq 0 (
        echo ERROR: Installation failed
        cd ..
        goto :error_exit
    )
    echo ✅ Installation successful
    echo.
)

:: Return to source directory
cd ..

:: Post-build steps
if "%INSTALL_AFTER_BUILD%"=="true" (
    echo Performing post-build setup...
    
    :: Copy configuration files
    if exist "config" (
        xcopy "config" "%INSTALL_DIR%\config\" /E /I /Y >nul 2>&1
        echo ✅ Configuration files copied
    )
    
    :: Copy profiles
    if exist "profiles" (
        xcopy "profiles" "%INSTALL_DIR%\profiles\" /E /I /Y >nul 2>&1
        echo ✅ Profile files copied
    )
    
    :: Copy documentation
    if exist "docs" (
        xcopy "docs" "%INSTALL_DIR%\docs\" /E /I /Y >nul 2>&1
        echo ✅ Documentation copied
    )
    
    :: Copy source files for reference
    if exist "src" (
        xcopy "src" "%INSTALL_DIR%\src\" /E /I /Y >nul 2>&1
        echo ✅ Source files copied
    )
    
    :: Copy test files
    if exist "test_*.cpp" (
        copy "test_*.cpp" "%INSTALL_DIR%\tests\" >nul 2>&1
        echo ✅ Test files copied
    )
    
    :: Copy PowerShell test scripts
    if exist "test_*.ps1" (
        copy "test_*.ps1" "%INSTALL_DIR%\tests\" >nul 2>&1
        echo ✅ Test scripts copied
    )
    
    :: Copy batch test runner
    if exist "run_complete_test_suite.bat" (
        copy "run_complete_test_suite.bat" "%INSTALL_DIR%\" >nul 2>&1
        echo ✅ Test runner copied
    )
    
    :: Create launcher scripts
    if "%CREATE_LAUNCHERS%"=="true" (
        echo Creating launcher scripts...
        
        :: Main launcher
        echo @echo off > "%INSTALL_DIR%\launch_uevr.bat"
        echo echo Starting UEVR System... >> "%INSTALL_DIR%\launch_uevr.bat"
        echo cd /d "%%~dp0" >> "%INSTALL_DIR%\launch_uevr.bat"
        echo if exist "uevr_full_system.dll" ( >> "%INSTALL_DIR%\launch_uevr.bat"
        echo     echo Loading UEVR Full System... >> "%INSTALL_DIR%\launch_uevr.bat"
        echo     rundll32 uevr_full_system.dll,Initialize >> "%INSTALL_DIR%\launch_uevr.bat"
        echo ) else ( >> "%INSTALL_DIR%\launch_uevr.bat"
        echo     echo ERROR: UEVR system not found >> "%INSTALL_DIR%\launch_uevr.bat"
        echo     pause >> "%INSTALL_DIR%\launch_uevr.bat"
        echo ) >> "%INSTALL_DIR%\launch_uevr.bat"
        
        :: VR launcher
        echo @echo off > "%INSTALL_DIR%\launch_uevr_vr.bat"
        echo echo Starting UEVR VR System... >> "%INSTALL_DIR%\launch_uevr_vr.bat"
        echo cd /d "%%~dp0" >> "%INSTALL_DIR%\launch_uevr_vr.bat"
        echo if exist "uevr_full_system.dll" ( >> "%INSTALL_DIR%\launch_uevr_vr.bat"
        echo     echo Loading UEVR VR System... >> "%INSTALL_DIR%\launch_uevr_vr.bat"
        echo     rundll32 uevr_full_system.dll,InitializeVR >> "%INSTALL_DIR%\launch_uevr_vr.bat"
        echo ) else ( >> "%INSTALL_DIR%\launch_uevr_vr.bat"
        echo     echo ERROR: UEVR VR system not found >> "%INSTALL_DIR%\launch_uevr_vr.bat"
        echo     pause >> "%INSTALL_DIR%\launch_uevr_vr.bat"
        echo ) >> "%INSTALL_DIR%\launch_uevr_vr.bat"
        
        :: Test launcher
        echo @echo off > "%INSTALL_DIR%\run_tests.bat"
        echo echo Running UEVR Test Suite... >> "%INSTALL_DIR%\run_tests.bat"
        echo cd /d "%%~dp0" >> "%INSTALL_DIR%\run_tests.bat"
        echo if exist "run_complete_test_suite.bat" ( >> "%INSTALL_DIR%\run_tests.bat"
        echo     run_complete_test_suite.bat --full --clean >> "%INSTALL_DIR%\run_tests.bat"
        echo ) else ( >> "%INSTALL_DIR%\run_tests.bat"
        echo     echo ERROR: Test runner not found >> "%INSTALL_DIR%\run_tests.bat"
        echo     pause >> "%INSTALL_DIR%\run_tests.bat"
        echo ) >> "%INSTALL_DIR%\run_tests.bat"
        
        echo ✅ Launcher scripts created
    )
    
    :: Generate README
    echo Generating README...
    echo # UEVR System - Build Information > "%INSTALL_DIR%\README.md"
    echo. >> "%INSTALL_DIR%\README.md"
    echo **Build Date:** %DATE% %TIME% >> "%INSTALL_DIR%\README.md"
    echo **Build Type:** %BUILD_TYPE% >> "%INSTALL_DIR%\README.md"
    echo **Architecture:** %BUILD_ARCH% >> "%INSTALL_DIR%\README.md"
    echo **VR Support:** %ENABLE_VR% >> "%INSTALL_DIR%\README.md"
    echo **Vulkan Support:** %ENABLE_VULKAN% >> "%INSTALL_DIR%\README.md"
    echo **OpenXR Support:** %ENABLE_OPENXR% >> "%INSTALL_DIR%\README.md"
    echo **Testing:** %ENABLE_TESTING% >> "%INSTALL_DIR%\README.md"
    echo. >> "%INSTALL_DIR%\README.md"
    echo ## Quick Start >> "%INSTALL_DIR%\README.md"
    echo. >> "%INSTALL_DIR%\README.md"
    echo ### Launch System >> "%INSTALL_DIR%\README.md"
    echo ```bash >> "%INSTALL_DIR%\README.md"
    echo # Main system >> "%INSTALL_DIR%\README.md"
    echo launch_uevr.bat >> "%INSTALL_DIR%\README.md"
    echo. >> "%INSTALL_DIR%\README.md"
    echo # VR system >> "%INSTALL_DIR%\README.md"
    echo launch_uevr_vr.bat >> "%INSTALL_DIR%\README.md"
    echo. >> "%INSTALL_DIR%\README.md"
    echo # Run tests >> "%INSTALL_DIR%\README.md"
    echo run_tests.bat >> "%INSTALL_DIR%\README.md"
    echo ``` >> "%INSTALL_DIR%\README.md"
    echo. >> "%INSTALL_DIR%\README.md"
    echo ## Features >> "%INSTALL_DIR%\README.md"
    if "%BUILD_FULL_SYSTEM%"=="true" (
        echo - ✅ Full System with all adapters >> "%INSTALL_DIR%\README.md"
        echo - ✅ RE Engine, REDengine4, MT Framework support >> "%INSTALL_DIR%\README.md"
        echo - ✅ DirectX 11/12 and Vulkan hooks >> "%INSTALL_DIR%\README.md"
        echo - ✅ OpenXR integration >> "%INSTALL_DIR%\README.md"
        echo - ✅ UObjectHook component system >> "%INSTALL_DIR%\README.md"
        echo - ✅ Cross-Engine Adapter Factory >> "%INSTALL_DIR%\README.md"
    ) else (
        echo - ✅ Basic UEVR functionality >> "%INSTALL_DIR%\README.md"
        echo - ✅ Core VR adapter system >> "%INSTALL_DIR%\README.md"
    )
    echo ✅ README generated
    echo.
)

:: Build summary
echo.
echo ========================================
echo 🚀 UEVR Build Complete!
echo ========================================
echo.
echo Build Type: %BUILD_TYPE%
echo Architecture: %BUILD_ARCH%
echo VR Support: %ENABLE_VR%
echo Vulkan Support: %ENABLE_VULKAN%
echo OpenXR Support: %ENABLE_OPENXR%
echo Testing: %ENABLE_TESTING%
echo Full System: %BUILD_FULL_SYSTEM%
echo.
echo Build Directory: %BUILD_DIR%
if "%INSTALL_AFTER_BUILD%"=="true" (
    echo Install Directory: %INSTALL_DIR%
    echo.
    echo Next Steps:
    echo 1. Test the system: run_complete_test_suite.bat --full --clean
    echo 2. Launch: %INSTALL_DIR%\launch_uevr.bat
    echo 3. VR Launch: %INSTALL_DIR%\launch_uevr_vr.bat
    echo 4. Run Tests: %INSTALL_DIR%\run_tests.bat
    echo.
    echo Would you like to open the install directory? (Y/N)
    set /p OPEN_DIR=
    if /i "!OPEN_DIR!"=="Y" (
        explorer "%INSTALL_DIR%"
    )
)
echo.
echo Build completed successfully! 🎉
goto :end

:show_help
echo.
echo UEVR Complete System Build Script - Usage
echo =========================================
echo.
echo build_complete_system.bat [options]
echo.
echo Options:
echo   --debug          Build in debug mode with sanitizers
echo   --release        Build in release mode (default)
echo   --x86            Build for 32-bit architecture
echo   --x64            Build for 64-bit architecture (default)
echo   --novr           Disable VR support
echo   --novulkan       Disable Vulkan support
echo   --noopenxr       Disable OpenXR support
echo   --notest         Disable testing
echo   --clean          Clean build directory before building
echo   --verbose        Enable verbose output
echo   --minimal        Build minimal system (not full system)
echo   --noinstall      Skip installation step
echo   --nolaunchers    Skip launcher script creation
echo   --help           Show this help message
echo.
echo Examples:
echo   build_complete_system.bat --verbose
echo   build_complete_system.bat --debug --clean
echo   build_complete_system.bat --minimal --novr
echo.
goto :end

:error_exit
echo.
echo ❌ Build failed! Please check the error messages above.
echo.
pause
exit /b 1

:end
echo.
pause
