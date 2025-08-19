@echo off
setlocal enabledelayedexpansion

echo ========================================
echo RE-Engine Cross-Engine VR Adapter Build
echo ========================================

REM Check if Visual Studio is available
where cl >nul 2>&1
if %ERRORLEVEL% neq 0 (
    echo WARNING: Visual Studio compiler not found in PATH
    echo Attempting to use Visual Studio Developer Command Prompt...
    
    REM Try to find and run Visual Studio Developer Command Prompt
    set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
    if exist "%VSWHERE%" (
        for /f "tokens=*" %%i in ('"%VSWHERE%" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath') do (
            set "VS_PATH=%%i
        )
        if defined VS_PATH (
            echo Found Visual Studio at: %VS_PATH%
            call "%VS_PATH%\VC\Auxiliary\Build\vcvars64.bat"
        ) else (
            echo ERROR: Visual Studio with C++ tools not found!
            pause
            exit /b 1
        )
    ) else (
        echo ERROR: vswhere.exe not found! Please install Visual Studio.
        pause
        exit /b 1
    )
)

echo Building RE-Engine Cross-Engine VR Adapter...

REM Create build directory
if not exist build mkdir build
cd build

REM Configure with CMake
echo [1/3] Configuring with CMake...
cmake .. -G "Visual Studio 17 2022" -A x64
if %ERRORLEVEL% neq 0 (
    echo ERROR: CMake configuration failed!
    pause
    exit /b 1
)

REM Build the adapter
echo [2/3] Building RE-Engine Adapter...
cmake --build . --config Release
if %ERRORLEVEL% neq 0 (
    echo ERROR: Build failed!
    pause
    exit /b 1
)

echo [3/3] Build completed successfully!
echo Output: re_engine_adapter.dll

cd ..
echo.
echo ========================================
echo RE-Engine Adapter Build Complete!
echo ========================================
pause
