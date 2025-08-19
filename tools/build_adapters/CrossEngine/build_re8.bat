@echo off
REM RE8 Cross-Engine VR Adapter Build Script
REM Builds RE8 support for uevr Cross-Engine system

echo.
echo ========================================
echo RE8 Cross-Engine VR Adapter Build
echo ========================================
echo.

REM Set build environment
set BUILD_DIR=build_re8
set SOURCE_DIR=src\adapters
set OUTPUT_DIR=install_re8

REM Create build directory
if not exist "%BUILD_DIR%" mkdir "%BUILD_DIR%"
cd "%BUILD_DIR%"

REM Configure CMake for RE8
echo Configuring CMake for RE8...
cmake ..\..\%SOURCE_DIR% -G "Visual Studio 17 2022" -A x64 ^
    -Duevr_BUILD_RE8_ADAPTER=ON ^
    -Duevr_BUILD_CROSS_ENGINE=ON ^
    -Duevr_BUILD_ALL_ADAPTERS=ON ^
    -DCMAKE_BUILD_TYPE=Release

if %ERRORLEVEL% neq 0 (
    echo ERROR: CMake configuration failed!
    pause
    exit /b 1
)

REM Build RE8 adapter
echo Building RE8 adapter...
cmake --build . --config Release --target uevr_re_engine_adapter

if %ERRORLEVEL% neq 0 (
    echo ERROR: Build failed!
    pause
    exit /b 1
)

REM Build all cross-engine adapters
echo Building all cross-engine adapters...
cmake --build . --config Release --target uevr_redengine4_adapter
cmake --build . --config Release --target uevr_mt_framework_adapter

REM Create output directory
if not exist "..\..\%OUTPUT_DIR%" mkdir "..\..\%OUTPUT_DIR%"

REM Copy built adapters
echo Copying built adapters...
copy "Release\*.dll" "..\..\%OUTPUT_DIR%\"
copy "Release\*.lib" "..\..\%OUTPUT_DIR%\"

REM Copy RE8 profile
echo Copying RE8 profile...
xcopy "..\..\profiles\re8-Win64-Shipping" "..\..\%OUTPUT_DIR%\profiles\re8-Win64-Shipping\" /E /I /Y

REM Copy documentation
echo Copying documentation...
copy "..\..\docs\cross-engine\*.md" "..\..\%OUTPUT_DIR%\docs\"

echo.
echo ========================================
echo RE8 Build Complete!
echo ========================================
echo.
echo Built adapters:
echo - RE Engine Adapter (RE7 + RE8)
echo - REDengine 4 Adapter (Cyberpunk 2077)
echo - MT Framework Adapter (Monster Hunter World)
echo.
echo Output directory: %OUTPUT_DIR%
echo.

pause
