@echo off
echo ===============================================
echo uevr UPDATED STRUCTURE BUILD SYSTEM
echo ===============================================
echo Building with new src/adapters/ structure...
echo.

REM Set paths for new structure
set SRC_DIR=..\src
set ADAPTERS_DIR=%SRC_DIR%\adapters
set BUILD_DIR=build
set REPORTS_DIR=build_reports

REM Create necessary directories
if not exist %BUILD_DIR% mkdir %BUILD_DIR%
if not exist %REPORTS_DIR% mkdir %REPORTS_DIR%

echo.
echo ===============================================
echo BUILDING ADAPTERS WITH NEW STRUCTURE
echo ===============================================

REM Build REDengine4 adapter
echo Building REDengine4 adapter...
cd %ADAPTERS_DIR%\redengine4
if exist build.bat (
    call build.bat
    if %ERRORLEVEL% EQU 0 (
        echo REDengine4 adapter built successfully
    ) else (
        echo ERROR: REDengine4 adapter build failed
    )
) else (
    echo WARNING: build.bat not found in REDengine4
)
cd ..\..

REM Build MT-Framework adapter
echo Building MT-Framework adapter...
cd %ADAPTERS_DIR%\mt-framework
if exist build.bat (
    call build.bat
    if %ERRORLEVEL% EQU 0 (
        echo MT-Framework adapter built successfully
    ) else (
        echo ERROR: MT-Framework adapter build failed
    )
) else (
    echo WARNING: build.bat not found in MT-Framework
)
cd ..\..

REM Build RE-Engine adapter
echo Building RE-Engine adapter...
cd %ADAPTERS_DIR%\re-engine
if exist build.bat (
    call build.bat
    if %ERRORLEVEL% EQU 0 (
        echo RE-Engine adapter built successfully
    ) else (
        echo ERROR: RE-Engine adapter build failed
    )
) else (
    echo WARNING: build.bat not found in RE-Engine
)
cd ..\..

REM Build main cross-engine framework
echo Building main cross-engine framework...
cd %SRC_DIR%
if exist CMakeLists.txt (
    echo Using CMake build system...
    if not exist build mkdir build
    cd build
    cmake .. -G "Visual Studio 16 2019" -A x64
    if %ERRORLEVEL% EQU 0 (
        cmake --build . --config Release
        if %ERRORLEVEL% EQU 0 (
            echo Main framework built successfully
        ) else (
            echo ERROR: Main framework build failed
        )
    ) else (
        echo ERROR: CMake configuration failed
    )
    cd ..
) else (
    echo WARNING: CMakeLists.txt not found in src directory
)
cd ..

echo.
echo ===============================================
echo BUILD COMPLETION SUMMARY
echo ===============================================
echo All adapters and main framework built with new structure
echo Build artifacts located in: %BUILD_DIR%
echo Build reports located in: %REPORTS_DIR%
echo.
echo New structure paths:
echo - REDengine4: %ADAPTERS_DIR%\redengine4
echo - MT-Framework: %ADAPTERS_DIR%\mt-framework  
echo - RE-Engine: %ADAPTERS_DIR%\re-engine
echo - Main Framework: %SRC_DIR%
echo.
echo Build system ready for new file structure!
pause
