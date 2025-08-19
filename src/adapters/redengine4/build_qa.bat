@echo off
REM Cyberpunk 2077 REDengine 4 Adapter - QA Build Script
REM Builds with warnings-as-errors and performs static analysis
REM This script ensures code quality and catches potential issues

echo ========================================
echo REDengine 4 Adapter QA Build Process
echo ========================================
echo.

REM Check prerequisites
where cmake >nul 2>nul
if %errorlevel% neq 0 (
    echo ERROR: CMake not found in PATH
    echo Please install CMake and add to PATH
    pause
    exit /b 1
)

where cl >nul 2>nul
if %errorlevel% neq 0 (
    echo WARNING: Visual Studio compiler not found in PATH
    echo Attempting to use Visual Studio Developer Command Prompt...
    echo.
    if exist "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat" (
        call "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat"
    ) else if exist "C:\Program Files\Microsoft Visual Studio\2022\Professional\Common7\Tools\VsDevCmd.bat" (
        call "C:\Program Files\Microsoft Visual Studio\2022\Professional\Common7\Tools\VsDevCmd.bat"
    ) else if exist "C:\Program Files\Microsoft Visual Studio\2022\Enterprise\Common7\Tools\VsDevCmd.bat" (
        call "C:\Program Files\Microsoft Visual Studio\2022\Enterprise\Common7\Tools\VsDevCmd.bat"
    ) else (
        echo ERROR: Visual Studio 2022 not found
        pause
        exit /b 1
    )
)

echo [1/5] Creating build directory...
if not exist "build_qa" mkdir build_qa
cd build_qa

echo [2/5] Configuring with CMake (Release, warnings-as-errors)...
cmake .. -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE=Release
if %errorlevel% neq 0 (
    echo ERROR: CMake configuration failed
    pause
    exit /b 1
)

echo [3/5] Building REDengine 4 Adapter (Release, warnings-as-errors)...
cmake --build . --target redengine4_adapter --config Release
if %errorlevel% neq 0 (
    echo ERROR: Build failed - check for warnings-as-errors
    echo.
    echo Common issues:
    echo - Unused variables (remove or mark as unused)
    echo - Missing return statements
    echo - Uninitialized variables
    echo - Type conversion warnings
    echo.
    pause
    exit /b 1
)

echo [4/5] Static analysis and code quality checks...
echo.
echo Checking for common code quality issues...

REM Check for TODO comments that might indicate incomplete implementation
echo - Scanning for TODO comments...
findstr /s /i "TODO" ..\*.cpp ..\*.hpp
if %errorlevel% equ 0 (
    echo WARNING: TODO comments found - review for incomplete implementation
) else (
    echo - No TODO comments found
)

REM Check for FIXME comments
echo - Scanning for FIXME comments...
findstr /s /i "FIXME" ..\*.cpp ..\*.hpp
if %errorlevel% equ 0 (
    echo WARNING: FIXME comments found - review for known issues
) else (
    echo - No FIXME comments found
)

REM Check for hardcoded values that might need configuration
echo - Scanning for potential hardcoded values...
findstr /s /n "0x[0-9A-Fa-f]\{8,16\}" ..\*.cpp ..\*.hpp
if %errorlevel% equ 0 (
    echo INFO: Hardcoded addresses found - verify these are correct
) else (
    echo - No hardcoded addresses found
)

echo.
echo [5/5] Installing and copying files...
if exist "..\..\..\profiles" (
    if not exist "..\..\..\profiles\adapters" mkdir "..\..\..\profiles\adapters"
    if not exist "..\..\..\profiles\adapters\redengine4" mkdir "..\..\..\profiles\adapters\redengine4"
    
    if exist "bin\Release\redengine4_adapter.dll" (
        copy "bin\Release\redengine4_adapter.dll" "..\..\..\profiles\adapters\redengine4\" >nul
        echo - Copied redengine4_adapter.dll to profiles\adapters\redengine4\
    )
    
    REM Copy source files for reference
    if exist "..\redengine_adapter.cpp" (
        copy "..\redengine_adapter.cpp" "..\..\..\profiles\adapters\redengine4\" >nul
        echo - Copied redengine_adapter.cpp to profiles\adapters\redengine4\
    )
    if exist "..\redengine_adapter.hpp" (
        copy "..\redengine_adapter.hpp" "..\..\..\profiles\adapters\redengine4\" >nul
        echo - Copied redengine_adapter.hpp to profiles\adapters\redengine4\
    )
    if exist "..\uobject_hook.cpp" (
        copy "..\uobject_hook.cpp" "..\..\..\profiles\adapters\redengine4\" >nul
        echo - Copied uobject_hook.cpp to profiles\adapters\redengine4\
    )
    if exist "..\uobject_hook.hpp" (
        copy "..\uobject_hook.hpp" "..\..\..\profiles\adapters\redengine4\" >nul
        echo - Copied uobject_hook.hpp to profiles\adapters\redengine4\
    )
    
    echo.
) else (
    echo WARNING: profiles directory not found, skipping installation
)

echo.
echo ========================================
echo QA Build completed successfully!
echo ========================================
echo.
echo Build Summary:
echo - Configuration: Release with warnings-as-errors
echo - Compiler: Visual Studio 2022 (MSVC)
echo - Warnings: Treated as errors (WX flag)
echo - Output: redengine4_adapter.dll
echo.
echo Quality Checks:
echo - Build completed without warnings-as-errors
echo - Static analysis performed
echo - Code quality issues identified (if any)
echo.
echo Next Steps:
echo 1. Verify DLL loads correctly in uevr
echo 2. Test injection and hook installation
echo 3. Validate VR rendering functionality
echo 4. Test UObjectHook system
echo.
if exist "bin\Release" (
    echo Built files:
    dir /b "bin\Release\*.dll"
    echo.
)

echo QA build script completed successfully!
echo.
pause
