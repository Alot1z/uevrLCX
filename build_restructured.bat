@echo off
setlocal enabledelayedexpansion

echo ========================================
echo UEVR Advanced Build System
echo ========================================
echo.

:: Set build configuration
set BUILD_TYPE=Release
set BUILD_ARCH=x64
set BUILD_PLATFORM=Win64
set ENABLE_VR=true
set ENABLE_DEBUG=false

:: Parse command line arguments
for %%a in (%*) do (
    if "%%a"=="--debug" set ENABLE_DEBUG=true
    if "%%a"=="--release" set BUILD_TYPE=Release
    if "%%a"=="--x86" set BUILD_ARCH=x86
    if "%%a"=="--novr" set ENABLE_VR=false
)

:: Set build directory
set BUILD_DIR=build_%BUILD_ARCH%_%BUILD_TYPE%
set INSTALL_DIR=install_%BUILD_ARCH%_%BUILD_TYPE%

:: Create build directories
if not exist "%BUILD_DIR%" mkdir "%BUILD_DIR%"
if not exist "%INSTALL_DIR%" mkdir "%INSTALL_DIR%"

echo Building UEVR for %BUILD_PLATFORM% (%BUILD_TYPE%)
echo Build directory: %BUILD_DIR%
echo Install directory: %INSTALL_DIR%
echo VR Support: %ENABLE_VR%
echo Debug Mode: %ENABLE_DEBUG%
echo.

:: Check for required tools
echo Checking build dependencies...
where cmake >nul 2>&1
if %ERRORLEVEL% neq 0 (
    echo ERROR: CMake not found in PATH
    echo Please install CMake and add it to your PATH
    pause
    exit /b 1
)

where cl >nul 2>&1
if %ERRORLEVEL% neq 0 (
    echo ERROR: Visual Studio compiler not found
    echo Please run this script from a Visual Studio Developer Command Prompt
    pause
    exit /b 1
)

:: Set CMake configuration
set CMAKE_CONFIG=-DCMAKE_BUILD_TYPE=%BUILD_TYPE%
set CMAKE_CONFIG=%CMAKE_CONFIG% -DCMAKE_INSTALL_PREFIX=%CD%\%INSTALL_DIR%
set CMAKE_CONFIG=%CMAKE_CONFIG% -DCMAKE_GENERATOR_PLATFORM=%BUILD_ARCH%

:: Add VR-specific options
if "%ENABLE_VR%"=="true" (
    set CMAKE_CONFIG=%CMAKE_CONFIG% -DENABLE_VR=ON
    set CMAKE_CONFIG=%CMAKE_CONFIG% -DENABLE_OPENXR=ON
    set CMAKE_CONFIG=%CMAKE_CONFIG% -DENABLE_OPENVR=ON
) else (
    set CMAKE_CONFIG=%CMAKE_CONFIG% -DENABLE_VR=OFF
)

:: Add debug options
if "%ENABLE_DEBUG%"=="true" (
    set CMAKE_CONFIG=%CMAKE_CONFIG% -DENABLE_DEBUG=ON
    set CMAKE_CONFIG=%CMAKE_CONFIG% -DENABLE_SYMBOLS=ON
)

:: Add dependency paths
set CMAKE_CONFIG=%CMAKE_CONFIG% -DMINHOOK_DIR=%CD%/external/minhook
set CMAKE_CONFIG=%CMAKE_CONFIG% -DNLOHMANN_JSON_DIR=%CD%/external/nlohmann_json
set CMAKE_CONFIG=%CMAKE_CONFIG% -DDIRECTX_SDK_DIR=%DXSDK_DIR%

echo CMake configuration: %CMAKE_CONFIG%
echo.

:: Configure project
echo Configuring project with CMake...
cd "%BUILD_DIR%"
cmake .. %CMAKE_CONFIG%
if %ERRORLEVEL% neq 0 (
    echo ERROR: CMake configuration failed
    cd ..
    pause
    exit /b 1
)

:: Build project
echo.
echo Building project...
cmake --build . --config %BUILD_TYPE% --parallel
if %ERRORLEVEL% neq 0 (
    echo ERROR: Build failed
    cd ..
    pause
    exit /b 1
)

:: Install project
echo.
echo Installing project...
cmake --install . --config %BUILD_TYPE%
if %ERRORLEVEL% neq 0 (
    echo ERROR: Installation failed
    cd ..
    pause
    exit /b 1
)

cd ..

:: Copy additional files
echo.
echo Copying additional files...
if not exist "%INSTALL_DIR%\bin" mkdir "%INSTALL_DIR%\bin"
if not exist "%INSTALL_DIR%\config" mkdir "%INSTALL_DIR%\config"
if not exist "%INSTALL_DIR%\profiles" mkdir "%INSTALL_DIR%\profiles"

:: Copy configuration files
copy "config\*.json" "%INSTALL_DIR%\config\" >nul 2>&1
copy "profiles\*.json" "%INSTALL_DIR%\profiles\" >nul 2>&1

:: Copy documentation
if not exist "%INSTALL_DIR%\docs" mkdir "%INSTALL_DIR%\docs"
xcopy "docs\*" "%INSTALL_DIR%\docs\" /E /I /Y >nul 2>&1

:: Create launcher script
echo @echo off > "%INSTALL_DIR%\launch_uevr.bat"
echo echo Starting UEVR Advanced VR System... >> "%INSTALL_DIR%\launch_uevr.bat"
echo cd /d "%%~dp0" >> "%INSTALL_DIR%\launch_uevr.bat"
echo start "" "bin\uevr.exe" >> "%INSTALL_DIR%\launch_uevr.bat"

:: Create VR launcher script
if "%ENABLE_VR%"=="true" (
    echo @echo off > "%INSTALL_DIR%\launch_uevr_vr.bat"
    echo echo Starting UEVR in VR Mode... >> "%INSTALL_DIR%\launch_uevr_vr.bat"
    echo cd /d "%%~dp0" >> "%INSTALL_DIR%\launch_uevr_vr.bat"
    echo set UEVR_VR_MODE=1 >> "%INSTALL_DIR%\launch_uevr_vr.bat"
    echo start "" "bin\uevr.exe" --vr >> "%INSTALL_DIR%\launch_uevr_vr.bat"
)

:: Build summary
echo.
echo ========================================
echo Build completed successfully!
echo ========================================
echo.
echo Build Type: %BUILD_TYPE%
echo Architecture: %BUILD_ARCH%
echo VR Support: %ENABLE_VR%
echo Debug Mode: %ENABLE_DEBUG%
echo.
echo Files installed to: %INSTALL_DIR%
echo.
echo To run UEVR:
echo   - Standard mode: %INSTALL_DIR%\launch_uevr.bat
if "%ENABLE_VR%"=="true" (
    echo   - VR mode: %INSTALL_DIR%\launch_uevr_vr.bat
)
echo.
echo To clean build files: rmdir /s /q "%BUILD_DIR%"
echo.

:: Optional: Open install directory
set /p OPEN_DIR="Open install directory? (y/n): "
if /i "!OPEN_DIR!"=="y" (
    start "" "%INSTALL_DIR%"
)

pause
