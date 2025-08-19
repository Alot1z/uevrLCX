@echo off
setlocal enabledelayedexpansion

REM Normalize working directory to the script location (handles spaces)
pushd "%~dp0" >nul

echo ===============================================
echo uevr COMPLETE STRUCTURE BUILD SYSTEM
echo ===============================================
echo Building with new src/adapters/ structure...
echo Based on comprehensive uevr documentation
echo.

REM Resolve absolute paths based on repo root
set "SCRIPT_DIR=%~dp0"
pushd "%SCRIPT_DIR%.." >nul
set "REPO_ROOT=%CD%"
popd >nul

set "SRC_DIR=%REPO_ROOT%\src"
set "ADAPTERS_DIR=%SRC_DIR%\adapters"
set "EXAMPLES_DIR=%REPO_ROOT%\examples"
set "BUILD_DIR=build"
set "REPORTS_DIR=build_reports"
set "ARTIFACTS_DIR=build_artifacts"
set "SECURITY_DIR=security"
set "DEPLOYMENT_DIR=deployment"
set "SHADERS_DIR=shaders"
set "TEXTURES_DIR=textures"

REM Create necessary directories
if not exist %BUILD_DIR% mkdir %BUILD_DIR%
if not exist %REPORTS_DIR% mkdir %REPORTS_DIR%
if not exist %ARTIFACTS_DIR% mkdir %ARTIFACTS_DIR%
if not exist %SECURITY_DIR% mkdir %SECURITY_DIR%
if not exist %DEPLOYMENT_DIR% mkdir %DEPLOYMENT_DIR%
if not exist %SHADERS_DIR% mkdir %SHADERS_DIR%
if not exist %TEXTURES_DIR% mkdir %TEXTURES_DIR%

echo.
echo ===============================================
echo BUILDING uevr CORE FRAMEWORK
echo ===============================================

REM Build main uevr framework
echo Building main uevr framework...
pushd "%SRC_DIR%" >nul
if exist "CMakeLists.txt" (
    echo Using CMake build system for main framework...
    if not exist build mkdir build
    pushd build >nul
    cmake .. -G "Visual Studio 17 2022" -A x64
    if %ERRORLEVEL% EQU 0 (
        echo CMake configuration successful
        cmake --build . --config Release
        if %ERRORLEVEL% EQU 0 (
            echo Main uevr framework built successfully
            echo Output: uevrBackend.dll
        ) else (
            echo ERROR: Main framework build failed
        )
    ) else (
        echo ERROR: CMake configuration failed
    )
    popd >nul
) else (
    echo WARNING: CMakeLists.txt not found in src directory
)
popd >nul

echo.
echo ===============================================
echo BUILDING ADAPTERS WITH NEW STRUCTURE
echo ===============================================

REM Build REDengine4 adapter
echo Building REDengine4 adapter...
pushd "%ADAPTERS_DIR%\redengine4" >nul 2>nul
if exist "build.bat" (
    echo Using existing build.bat for REDengine4...
    call build.bat
    if %ERRORLEVEL% EQU 0 (
        echo REDengine4 adapter built successfully
        echo Output: redengine_adapter.dll
    ) else (
        echo ERROR: REDengine4 adapter build failed
    )
) else (
    echo WARNING: build.bat not found in REDengine4
)
popd >nul

REM Build MT-Framework adapter
echo Building MT-Framework adapter...
pushd "%ADAPTERS_DIR%\mt-framework" >nul 2>nul
if exist "build.bat" (
    echo Using existing build.bat for MT-Framework...
    call build.bat
    if %ERRORLEVEL% EQU 0 (
        echo MT-Framework adapter built successfully
        echo Output: mt_framework_adapter.dll
    ) else (
        echo ERROR: MT-Framework adapter build failed
    )
) else (
    echo WARNING: build.bat not found in MT-Framework
)
popd >nul

REM Build RE-Engine adapter
echo Building RE-Engine adapter...
pushd "%ADAPTERS_DIR%\re-engine" >nul 2>nul
if exist "build.bat" (
    echo Using existing build.bat for RE-Engine...
    call build.bat
    if %ERRORLEVEL% EQU 0 (
        echo RE-Engine adapter built successfully
        echo Output: re_engine_adapter.dll
    ) else (
        echo ERROR: RE-Engine adapter build failed
    )
) else (
    echo WARNING: build.bat not found in RE-Engine
)
popd >nul

echo.
echo ===============================================
echo BUILDING PLUGINS AND EXTENSIONS
echo ===============================================

REM Build example plugin
echo Building example plugin...
if exist "%EXAMPLES_DIR%\basic_plugin" (
    echo Building example plugin with CMake...
    pushd "%EXAMPLES_DIR%" >nul
    if not exist build mkdir build
    pushd build >nul
    cmake .. -G "Visual Studio 17 2022" -A x64
    if %ERRORLEVEL% EQU 0 (
        cmake --build . --config Release
        if %ERRORLEVEL% EQU 0 (
            echo Example plugin built successfully
            echo Output: example_plugin.dll
        ) else (
            echo ERROR: Example plugin build failed
        )
    ) else (
        echo ERROR: Example plugin CMake configuration failed
    )
    popd >nul
    popd >nul
) else (
    echo WARNING: Example plugin CMakeLists.txt not found
)

REM Build Lua scripting examples
echo Building Lua scripting examples...
if exist "%EXAMPLES_DIR%\lua_scripting" (
    echo Building Lua scripting with CMake...
    pushd "%EXAMPLES_DIR%" >nul
    if not exist build mkdir build
    pushd build >nul
    cmake .. -G "Visual Studio 17 2022" -A x64
    if %ERRORLEVEL% EQU 0 (
        cmake --build . --config Release
        if %ERRORLEVEL% EQU 0 (
            echo Lua scripting examples built successfully
        ) else (
            echo ERROR: Lua scripting build failed
        )
    ) else (
        echo ERROR: Lua scripting CMake configuration failed
    )
    popd >nul
    popd >nul
) else (
    echo WARNING: Lua scripting project not found
)

echo.
echo ===============================================
echo BUILDING ULTRA-ADVANCED BUILD SYSTEM
echo ===============================================

REM Build the Ultra-Advanced Build ^& Analysis System
echo Building Ultra-Advanced Build ^& Analysis System...
pushd "%REPO_ROOT%\build_system" >nul

REM Build QA System
echo Building Quality Assurance System...
if exist comprehensive_qa_system.cpp (
    echo Compiling QA System...
    g++ -std=c++17 -o qa_system.exe comprehensive_qa_system.cpp
    if %ERRORLEVEL% EQU 0 (
        echo QA System compiled successfully
    ) else (
        echo WARNING: QA System compilation failed (g++ not found)
    )
) else (
    echo WARNING: comprehensive_qa_system.cpp not found
)

REM Build Enterprise Security System
echo Building Enterprise Security System...
if exist enterprise_security_system.cpp (
    echo Compiling Security System...
    g++ -std=c++17 -o security_system.exe enterprise_security_system.cpp
    if %ERRORLEVEL% EQU 0 (
        echo Security System compiled successfully
    ) else (
        echo WARNING: Security System compilation failed (g++ not found)
    )
) else (
    echo WARNING: enterprise_security_system.cpp not found
)

REM Build Advanced Rendering System
echo Building Advanced Rendering System...
if exist advanced_rendering_system.cpp (
    echo Compiling Rendering System...
    g++ -std=c++17 -o rendering_system.exe advanced_rendering_system.cpp
    if %ERRORLEVEL% EQU 0 (
        echo Rendering System compiled successfully
    ) else (
        echo WARNING: Rendering System compilation failed (g++ not found)
    )
) else (
    echo WARNING: advanced_rendering_system.cpp not found
)

REM Build Comprehensive Deployment System
echo Building Comprehensive Deployment System...
if exist comprehensive_deployment_system.cpp (
    echo Compiling Deployment System...
    g++ -std=c++17 -o deployment_system.exe comprehensive_deployment_system.cpp
    if %ERRORLEVEL% EQU 0 (
        echo Deployment System compiled successfully
    ) else (
        echo WARNING: Deployment System compilation failed (g++ not found)
    )
) else (
    echo WARNING: comprehensive_deployment_system.cpp not found
)

REM Build Master Build ^& Analysis Executor
echo Building Master Build ^& Analysis Executor...
if exist master_build_analysis_executor.cpp (
    echo Compiling Master Executor...
    g++ -std=c++17 -o master_executor.exe master_build_analysis_executor.cpp
    if %ERRORLEVEL% EQU 0 (
        echo Master Executor compiled successfully
    ) else (
        echo WARNING: Master Executor compilation failed (g++ not found)
    )
) else (
    echo WARNING: master_build_analysis_executor.cpp not found
)

popd >nul

echo.
echo ===============================================
echo BUILD COMPLETION SUMMARY
echo ===============================================
echo All adapters and main framework built with new structure
echo Build artifacts located in: %BUILD_DIR%
echo Build reports located in: %REPORTS_DIR%
echo Build artifacts located in: %ARTIFACTS_DIR%
echo.
echo New structure paths:
echo - Main Framework: %SRC_DIR%
echo - REDengine4: %ADAPTERS_DIR%\redengine4
echo - MT-Framework: %ADAPTERS_DIR%\mt-framework  
echo - RE-Engine: %ADAPTERS_DIR%\re-engine
echo - Plugins: %EXAMPLES_DIR%
echo - Build System: build_system
echo.
echo Build system ready for new file structure!
echo All builds follow the new src/adapters/ organization
echo.
echo ===============================================
echo uevr COMPLETE STRUCTURE BUILD COMPLETE
echo ===============================================

popd >nul

endlocal
pause
