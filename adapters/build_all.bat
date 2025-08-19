@echo off
REM UEVR Cross-Engine VR Adapters - Complete Build System
REM Builds all 4 components: Main Framework + 3 Engine Adapters
REM Copyright (c) 2024 UEVR Cross-Engine Adapter Project

echo ========================================
echo UEVR Cross-Engine VR Adapters Build
echo ========================================
echo.

REM Set build configuration
set BUILD_TYPE=Release
set CMAKE_FLAGS=/W4 /WX
set PARALLEL_JOBS=8

echo Build Configuration:
echo - Build Type: %BUILD_TYPE%
echo - Compiler Flags: %CMAKE_FLAGS%
echo - Parallel Jobs: %PARALLEL_JOBS%
echo.

REM Create build directory
if not exist build mkdir build
cd build

echo ========================================
echo Building Main Cross-Engine Framework
echo ========================================
echo.

REM Build main cross-engine framework
cd ..\..\SOURCECODE\UEVR
if not exist build mkdir build
cd build

echo Configuring main framework...
cmake .. -DCMAKE_BUILD_TYPE=%BUILD_TYPE% -DCMAKE_CXX_FLAGS="%CMAKE_FLAGS%"
if errorlevel 1 (
    echo ERROR: Main framework configuration failed
    exit /b 1
)

echo Building main framework...
cmake --build . --config %BUILD_TYPE% --parallel %PARALLEL_JOBS%
if errorlevel 1 (
    echo ERROR: Main framework build failed
    exit /b 1
)

echo Main framework build completed successfully
cd ..\..\..\adapters

echo.
echo ========================================
echo Building MT Framework Adapter
echo ========================================
echo.

REM Build MT Framework adapter
cd ..\src\adapters\mt-framework
if not exist build mkdir build
cd build

echo Configuring MT Framework adapter...
cmake .. -DCMAKE_BUILD_TYPE=%BUILD_TYPE% -DCMAKE_CXX_FLAGS="%CMAKE_FLAGS%"
if errorlevel 1 (
    echo ERROR: MT Framework adapter configuration failed
    exit /b 1
)

echo Building MT Framework adapter...
cmake --build . --config %BUILD_TYPE% --parallel %PARALLEL_JOBS%
if errorlevel 1 (
    echo ERROR: MT Framework adapter build failed
    exit /b 1
)

echo MT Framework adapter build completed successfully
cd ..\..\..\adapters

echo.
echo ========================================
echo Building RE Engine Adapter
echo ========================================
echo.

REM Build RE Engine adapter
cd ..\src\adapters\re-engine
if not exist build mkdir build
cd build

echo Configuring RE Engine adapter...
cmake .. -DCMAKE_BUILD_TYPE=%BUILD_TYPE% -DCMAKE_CXX_FLAGS="%CMAKE_FLAGS%"
if errorlevel 1 (
    echo ERROR: RE Engine adapter configuration failed
    exit /b 1
)

echo Building RE Engine adapter...
cmake --build . --config %BUILD_TYPE% --parallel %PARALLEL_JOBS%
if errorlevel 1 (
    echo ERROR: RE Engine adapter build failed
    exit /b 1
)

echo RE Engine adapter build completed successfully
cd ..\..\..\adapters

echo.
echo ========================================
echo Building REDengine 4 Adapter
echo ========================================
echo.

REM Build REDengine 4 adapter
cd ..\src\adapters\redengine4
if not exist build mkdir build
cd build

echo Configuring REDengine 4 adapter...
cmake .. -DCMAKE_BUILD_TYPE=%BUILD_TYPE% -DCMAKE_CXX_FLAGS="%CMAKE_FLAGS%"
if errorlevel 1 (
    echo ERROR: REDengine 4 adapter configuration failed
    exit /b 1
)

echo Building REDengine 4 adapter...
cmake --build . --config %BUILD_TYPE% --parallel %PARALLEL_JOBS%
if errorlevel 1 (
    echo ERROR: REDengine 4 adapter build failed
    exit /b 1
)

echo REDengine 4 adapter build completed successfully
cd ..\..\..\adapters

echo.
echo ========================================
echo Building Unified Cross-Engine System
echo ========================================
echo.

REM Build unified cross-engine system
cd ..
if not exist build mkdir build
cd build

echo Configuring unified cross-engine system...
cmake .. -DCMAKE_BUILD_TYPE=%BUILD_TYPE% -DCMAKE_CXX_FLAGS="%CMAKE_FLAGS%"
if errorlevel 1 (
    echo ERROR: Unified system configuration failed
    exit /b 1
)

echo Building unified cross-engine system...
cmake --build . --config %BUILD_TYPE% --parallel %PARALLEL_JOBS%
if errorlevel 1 (
    echo ERROR: Unified system build failed
    exit /b 1
)

echo Unified cross-engine system build completed successfully
cd ..

echo.
echo ========================================
echo Build Validation
echo ========================================
echo.

REM Validate all builds
echo Checking build outputs...
if exist "..\src\adapters\mt-framework\build\mt_framework_cross_engine_adapter.dll" (
    echo ✓ MT Framework adapter DLL found
) else (
    echo ✗ MT Framework adapter DLL missing
)

if exist "..\src\adapters\re-engine\build\RE_Engine_Adapter.dll" (
    echo ✓ RE Engine adapter DLL found (RE_Engine_Adapter.dll)
) else (
    echo ✗ RE Engine adapter DLL missing
)

if exist "..\src\adapters\redengine4\build\redengine4_adapter.dll" (
    echo ✓ REDengine 4 adapter DLL found
) else (
    echo ✗ REDengine 4 adapter DLL missing
)

if exist "build\uevr_cross_engine_system.dll" (
    echo ✓ Main cross-engine system DLL found
) else (
    echo ✗ Main cross-engine system DLL missing
)

echo.
echo ========================================
echo Deployment
echo ========================================
echo.

REM Deploy to profile directories
echo Deploying adapters to profile directories...

REM MT Framework to Monster Hunter World
if exist "..\src\adapters\mt-framework\build\mt_framework_cross_engine_adapter.dll" (
    copy "..\src\adapters\mt-framework\build\mt_framework_cross_engine_adapter.dll" "profiles\MonsterHunterWorld\" >nul
    echo ✓ MT Framework adapter deployed to Monster Hunter World profile
)

REM RE Engine to Resident Evil 7
if exist "..\src\adapters\re-engine\build\RE_Engine_Adapter.dll" (
    copy "..\src\adapters\re-engine\build\RE_Engine_Adapter.dll" "profiles\re7\" >nul
    echo ✓ RE Engine adapter deployed to Resident Evil 7 profile (RE_Engine_Adapter.dll)
)

REM REDengine 4 to Cyberpunk 2077
if exist "..\src\adapters\redengine4\build\redengine4_adapter.dll" (
    copy "..\src\adapters\redengine4\build\redengine4_adapter.dll" "profiles\Cyberpunk2077\" >nul
    echo ✓ REDengine 4 adapter deployed to Cyberpunk 2077 profile
)

echo.
echo ========================================
echo Build Summary
echo ========================================
echo.
echo All 4 components built successfully:
echo 1. ✓ Main Cross-Engine Framework
echo 2. ✓ MT Framework Adapter (Monster Hunter World)
echo 3. ✓ RE Engine Adapter (Resident Evil 7)
echo 4. ✓ REDengine 4 Adapter (Cyberpunk 2077)
echo.
echo Build completed successfully!
echo All adapters deployed to their respective profile directories.
echo.
pause
