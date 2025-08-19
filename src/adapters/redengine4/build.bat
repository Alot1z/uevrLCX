@echo off
REM Build REDengine 4 Cross-Engine VR Adapter for Cyberpunk 2077
REM This script builds the REDengine 4 adapter DLL for uevr integration

echo ========================================
echo REDengine 4 Cross-Engine VR Adapter Build
echo ========================================
echo.

REM Check if CMake is available
where cmake >nul 2>nul
if %errorlevel% neq 0 (
    echo ERROR: CMake not found in PATH
    echo Please install CMake and add it to your PATH
    pause
    exit /b 1
)

REM Check if Visual Studio is available
where cl >nul 2>nul
if %errorlevel% neq 0 (
    echo WARNING: Visual Studio compiler not found in PATH
    echo Attempting to use Visual Studio Developer Command Prompt...
    echo.
    
    REM Try to find and use Visual Studio Developer Command Prompt
    if exist "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat" (
        call "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat"
    ) else if exist "C:\Program Files\Microsoft Visual Studio\2022\Professional\Common7\Tools\VsDevCmd.bat" (
        call "C:\Program Files\Microsoft Visual Studio\2022\Professional\Common7\Tools\VsDevCmd.bat"
    ) else if exist "C:\Program Files\Microsoft Visual Studio\2022\Enterprise\Common7\Tools\VsDevCmd.bat" (
        call "C:\Program Files\Microsoft Visual Studio\2022\Enterprise\Common7\Tools\VsDevCmd.bat"
    ) else (
        echo ERROR: Visual Studio 2022 not found
        echo Please install Visual Studio 2022
        pause
        exit /b 1
    )
)

echo Building REDengine 4 Cross-Engine VR Adapter...
echo.

REM Create build directory
if not exist "build" mkdir build
cd build

REM Configure with CMake
echo [1/3] Configuring with CMake...
cmake .. -G "Visual Studio 17 2022" -A x64
if %errorlevel% neq 0 (
    echo ERROR: CMake configuration failed
    pause
    exit /b 1
)

REM Build REDengine 4 Adapter
echo [2/3] Building REDengine 4 Adapter...
cmake --build . --target redengine4_adapter --config Release
if %errorlevel% neq 0 (
    echo ERROR: REDengine 4 Adapter build failed
    pause
    exit /b 1
)

REM Copy to profiles directory
echo [3/3] Installing adapter...
if exist "..\..\..\profiles" (
    if not exist "..\..\..\profiles\adapters" mkdir "..\..\..\profiles\adapters"
    if not exist "..\..\..\profiles\adapters\redengine4" mkdir "..\..\..\profiles\adapters\redengine4"
    
    if exist "bin\Release\redengine4_adapter.dll" (
        copy "bin\Release\redengine4_adapter.dll" "..\..\..\profiles\adapters\redengine4\" >nul
        echo - Copied redengine4_adapter.dll to profiles\adapters\redengine4\
    )
    
    if exist "redengine_adapter.cpp" (
        copy "redengine_adapter.cpp" "..\..\..\profiles\adapters\redengine4\" >nul
        echo - Copied redengine_adapter.cpp to profiles\adapters\redengine4\
    )
    
    if exist "redengine_adapter.hpp" (
        copy "redengine_adapter.hpp" "..\..\..\profiles\adapters\redengine4\" >nul
        echo - Copied redengine_adapter.hpp to profiles\adapters\redengine4\
    )
    
    echo.
) else (
    echo WARNING: profiles directory not found, skipping installation
)

echo.
echo ========================================
echo Build completed successfully!
echo ========================================
echo.
echo Built adapter: redengine4_adapter.dll
echo Output location: build\bin\Release\
echo.

REM List built files
if exist "bin\Release" (
    echo Built files:
    dir /b "bin\Release\*.dll"
    echo.
)

echo Build script completed successfully!
echo.
pause
