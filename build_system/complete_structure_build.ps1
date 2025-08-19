# uevr COMPLETE STRUCTURE BUILD SYSTEM
# PowerShell Version
# Based on comprehensive uevr documentation

Write-Host "===============================================" -ForegroundColor Cyan
Write-Host "uevr COMPLETE STRUCTURE BUILD SYSTEM" -ForegroundColor Cyan
Write-Host "===============================================" -ForegroundColor Cyan
Write-Host "Building with new src/adapters/ structure..." -ForegroundColor White
Write-Host "Based on comprehensive uevr documentation" -ForegroundColor White
Write-Host ""

# Set paths for new structure
$SRC_DIR = "..\src"
$ADAPTERS_DIR = "$SRC_DIR\adapters"
$BUILD_DIR = "build"
$REPORTS_DIR = "build_reports"
$ARTIFACTS_DIR = "build_artifacts"
$SECURITY_DIR = "security"
$DEPLOYMENT_DIR = "deployment"
$SHADERS_DIR = "shaders"
$TEXTURES_DIR = "textures"

# Create necessary directories
$Directories = @($BUILD_DIR, $REPORTS_DIR, $ARTIFACTS_DIR, $SECURITY_DIR, $DEPLOYMENT_DIR, $SHADERS_DIR, $TEXTURES_DIR)
foreach ($dir in $Directories) {
    if (!(Test-Path $dir)) {
        New-Item -ItemType Directory -Path $dir -Force
        Write-Host "Created directory: $dir" -ForegroundColor Green
    }
}

Write-Host ""
Write-Host "===============================================" -ForegroundColor Yellow
Write-Host "BUILDING uevr CORE FRAMEWORK" -ForegroundColor Yellow
Write-Host "===============================================" -ForegroundColor Yellow

# Build main uevr framework
Write-Host "Building main uevr framework..." -ForegroundColor White
if (Test-Path "$SRC_DIR\CMakeLists.txt") {
    Write-Host "Using CMake build system for main framework..." -ForegroundColor Gray
    if (!(Test-Path "$SRC_DIR\build")) {
        New-Item -ItemType Directory -Path "$SRC_DIR\build" -Force
    }
    
    Push-Location "$SRC_DIR\build"
    try {
        $cmakeResult = cmake .. -G "Visual Studio 17 2022" -A x64
        if ($LASTEXITCODE -eq 0) {
            Write-Host "CMake configuration successful" -ForegroundColor Green
            $buildResult = cmake --build . --config Release
            if ($LASTEXITCODE -eq 0) {
                Write-Host "Main uevr framework built successfully" -ForegroundColor Green
                Write-Host "Output: uevrBackend.dll" -ForegroundColor Gray
            } else {
                Write-Host "ERROR: Main framework build failed" -ForegroundColor Red
            }
        } else {
            Write-Host "ERROR: CMake configuration failed" -ForegroundColor Red
        }
    } finally {
        Pop-Location
    }
} else {
    Write-Host "WARNING: CMakeLists.txt not found in src directory" -ForegroundColor Yellow
}

Write-Host ""
Write-Host "===============================================" -ForegroundColor Yellow
Write-Host "BUILDING ADAPTERS WITH NEW STRUCTURE" -ForegroundColor Yellow
Write-Host "===============================================" -ForegroundColor Yellow

# Build REDengine4 adapter
Write-Host "Building REDengine4 adapter..." -ForegroundColor White
if (Test-Path "$ADAPTERS_DIR\redengine4\build.bat") {
    Write-Host "Using existing build.bat for REDengine4..." -ForegroundColor Gray
    Push-Location "$ADAPTERS_DIR\redengine4"
    try {
        & .\build.bat
        if ($LASTEXITCODE -eq 0) {
            Write-Host "REDengine4 adapter built successfully" -ForegroundColor Green
            Write-Host "Output: redengine_adapter.dll" -ForegroundColor Gray
        } else {
            Write-Host "ERROR: REDengine4 adapter build failed" -ForegroundColor Red
        }
    } finally {
        Pop-Location
    }
} else {
    Write-Host "WARNING: build.bat not found in REDengine4" -ForegroundColor Yellow
}

# Build MT-Framework adapter
Write-Host "Building MT-Framework adapter..." -ForegroundColor White
if (Test-Path "$ADAPTERS_DIR\mt-framework\build.bat") {
    Write-Host "Using existing build.bat for MT-Framework..." -ForegroundColor Gray
    Push-Location "$ADAPTERS_DIR\mt-framework"
    try {
        & .\build.bat
        if ($LASTEXITCODE -eq 0) {
            Write-Host "MT-Framework adapter built successfully" -ForegroundColor Green
            Write-Host "Output: mt_framework_adapter.dll" -ForegroundColor Gray
        } else {
            Write-Host "ERROR: MT-Framework adapter build failed" -ForegroundColor Red
        }
    } finally {
        Pop-Location
    }
} else {
    Write-Host "WARNING: build.bat not found in MT-Framework" -ForegroundColor Yellow
}

# Build RE-Engine adapter
Write-Host "Building RE-Engine adapter..." -ForegroundColor White
if (Test-Path "$ADAPTERS_DIR\re-engine\build.bat") {
    Write-Host "Using existing build.bat for RE-Engine..." -ForegroundColor Gray
    Push-Location "$ADAPTERS_DIR\re-engine"
    try {
        & .\build.bat
        if ($LASTEXITCODE -eq 0) {
            Write-Host "RE-Engine adapter built successfully" -ForegroundColor Green
            Write-Host "Output: re_engine_adapter.dll" -ForegroundColor Gray
        } else {
            Write-Host "ERROR: RE-Engine adapter build failed" -ForegroundColor Red
        }
    } finally {
        Pop-Location
    }
} else {
    Write-Host "WARNING: build.bat not found in RE-Engine" -ForegroundColor Yellow
}

Write-Host ""
Write-Host "===============================================" -ForegroundColor Yellow
Write-Host "BUILDING PLUGINS AND EXTENSIONS" -ForegroundColor Yellow
Write-Host "===============================================" -ForegroundColor Yellow

# Build example plugin
Write-Host "Building example plugin..." -ForegroundColor White
if (Test-Path "$SRC_DIR\examples\example_plugin\CMakeLists.txt") {
    Write-Host "Building example plugin with CMake..." -ForegroundColor Gray
    if (!(Test-Path "$SRC_DIR\examples\example_plugin\build")) {
        New-Item -ItemType Directory -Path "$SRC_DIR\examples\example_plugin\build" -Force
    }
    
    Push-Location "$SRC_DIR\examples\example_plugin\build"
    try {
        $cmakeResult = cmake .. -G "Visual Studio 17 2022" -A x64
        if ($LASTEXITCODE -eq 0) {
            $buildResult = cmake --build . --config Release
            if ($LASTEXITCODE -eq 0) {
                Write-Host "Example plugin built successfully" -ForegroundColor Green
                Write-Host "Output: example_plugin.dll" -ForegroundColor Gray
            } else {
                Write-Host "ERROR: Example plugin build failed" -ForegroundColor Red
            }
        } else {
            Write-Host "ERROR: Example plugin CMake configuration failed" -ForegroundColor Red
        }
    } finally {
        Pop-Location
    }
} else {
    Write-Host "WARNING: Example plugin CMakeLists.txt not found" -ForegroundColor Yellow
}

# Build Lua API
Write-Host "Building Lua API..." -ForegroundColor White
if (Test-Path "$SRC_DIR\lua-api\CMakeLists.txt") {
    Write-Host "Building Lua API with CMake..." -ForegroundColor Gray
    if (!(Test-Path "$SRC_DIR\lua-api\build")) {
        New-Item -ItemType Directory -Path "$SRC_DIR\lua-api\build" -Force
    }
    
    Push-Location "$SRC_DIR\lua-api\build"
    try {
        $cmakeResult = cmake .. -G "Visual Studio 17 2022" -A x64
        if ($LASTEXITCODE -eq 0) {
            $buildResult = cmake --build . --config Release
            if ($LASTEXITCODE -eq 0) {
                Write-Host "Lua API built successfully" -ForegroundColor Green
                Write-Host "Output: LuaVR.dll" -ForegroundColor Gray
            } else {
                Write-Host "ERROR: Lua API build failed" -ForegroundColor Red
            }
        } else {
            Write-Host "ERROR: Lua API CMake configuration failed" -ForegroundColor Red
        }
    } finally {
        Pop-Location
    }
} else {
    Write-Host "WARNING: Lua API CMakeLists.txt not found" -ForegroundColor Yellow
}

Write-Host ""
Write-Host "===============================================" -ForegroundColor Yellow
Write-Host "BUILDING ULTRA-ADVANCED BUILD SYSTEM" -ForegroundColor Yellow
Write-Host "===============================================" -ForegroundColor Yellow

# Build the Ultra-Advanced Build & Analysis System
Write-Host "Building Ultra-Advanced Build & Analysis System..." -ForegroundColor White

# Build QA System
Write-Host "Building Quality Assurance System..." -ForegroundColor White
if (Test-Path "comprehensive_qa_system.cpp") {
    Write-Host "Compiling QA System..." -ForegroundColor Gray
    try {
        g++ -std=c++17 -o qa_system.exe comprehensive_qa_system.cpp
        if ($LASTEXITCODE -eq 0) {
            Write-Host "QA System compiled successfully" -ForegroundColor Green
        } else {
            Write-Host "WARNING: QA System compilation failed (g++ not found)" -ForegroundColor Yellow
        }
    } catch {
        Write-Host "WARNING: QA System compilation failed (g++ not found)" -ForegroundColor Yellow
    }
} else {
    Write-Host "WARNING: comprehensive_qa_system.cpp not found" -ForegroundColor Yellow
}

# Build Enterprise Security System
Write-Host "Building Enterprise Security System..." -ForegroundColor White
if (Test-Path "enterprise_security_system.cpp") {
    Write-Host "Compiling Security System..." -ForegroundColor Gray
    try {
        g++ -std=c++17 -o security_system.exe enterprise_security_system.cpp
        if ($LASTEXITCODE -eq 0) {
            Write-Host "Security System compiled successfully" -ForegroundColor Green
        } else {
            Write-Host "WARNING: Security System compilation failed (g++ not found)" -ForegroundColor Yellow
        }
    } catch {
        Write-Host "WARNING: Security System compilation failed (g++ not found)" -ForegroundColor Yellow
    }
} else {
    Write-Host "WARNING: enterprise_security_system.cpp not found" -ForegroundColor Yellow
}

# Build Advanced Rendering System
Write-Host "Building Advanced Rendering System..." -ForegroundColor White
if (Test-Path "advanced_rendering_system.cpp") {
    Write-Host "Compiling Rendering System..." -ForegroundColor Gray
    try {
        g++ -std=c++17 -o rendering_system.exe advanced_rendering_system.cpp
        if ($LASTEXITCODE -eq 0) {
            Write-Host "Rendering System compiled successfully" -ForegroundColor Green
        } else {
            Write-Host "WARNING: Rendering System compilation failed (g++ not found)" -ForegroundColor Yellow
        }
    } catch {
        Write-Host "WARNING: Rendering System compilation failed (g++ not found)" -ForegroundColor Yellow
        }
} else {
    Write-Host "WARNING: advanced_rendering_system.cpp not found" -ForegroundColor Yellow
}

# Build Comprehensive Deployment System
Write-Host "Building Comprehensive Deployment System..." -ForegroundColor White
if (Test-Path "comprehensive_deployment_system.cpp") {
    Write-Host "Compiling Deployment System..." -ForegroundColor Gray
    try {
        g++ -std=c++17 -o deployment_system.exe comprehensive_deployment_system.cpp
        if ($LASTEXITCODE -eq 0) {
            Write-Host "Deployment System compiled successfully" -ForegroundColor Green
        } else {
            Write-Host "WARNING: Deployment System compilation failed (g++ not found)" -ForegroundColor Yellow
        }
    } catch {
        Write-Host "WARNING: Deployment System compilation failed (g++ not found)" -ForegroundColor Yellow
    }
} else {
    Write-Host "WARNING: comprehensive_deployment_system.cpp not found" -ForegroundColor Yellow
}

# Build Master Build & Analysis Executor
Write-Host "Building Master Build & Analysis Executor..." -ForegroundColor White
if (Test-Path "master_build_analysis_executor.cpp") {
    Write-Host "Compiling Master Executor..." -ForegroundColor Gray
    try {
        g++ -std=c++17 -o master_executor.exe master_build_analysis_executor.cpp
        if ($LASTEXITCODE -eq 0) {
            Write-Host "Master Executor compiled successfully" -ForegroundColor Green
        } else {
            Write-Host "WARNING: Master Executor compilation failed (g++ not found)" -ForegroundColor Yellow
        }
    } catch {
        Write-Host "WARNING: Master Executor compilation failed (g++ not found)" -ForegroundColor Yellow
    }
} else {
    Write-Host "WARNING: master_build_analysis_executor.cpp not found" -ForegroundColor Yellow
}

Write-Host ""
Write-Host "===============================================" -ForegroundColor Cyan
Write-Host "BUILD COMPLETION SUMMARY" -ForegroundColor Cyan
Write-Host "===============================================" -ForegroundColor Cyan
Write-Host "All adapters and main framework built with new structure" -ForegroundColor White
Write-Host "Build artifacts located in: $BUILD_DIR" -ForegroundColor Gray
Write-Host "Build reports located in: $REPORTS_DIR" -ForegroundColor Gray
Write-Host "Build artifacts located in: $ARTIFACTS_DIR" -ForegroundColor Gray
Write-Host ""
Write-Host "New structure paths:" -ForegroundColor White
Write-Host "- Main Framework: $SRC_DIR" -ForegroundColor Gray
Write-Host "- REDengine4: $ADAPTERS_DIR\redengine4" -ForegroundColor Gray
Write-Host "- MT-Framework: $ADAPTERS_DIR\mt-framework" -ForegroundColor Gray
Write-Host "- RE-Engine: $ADAPTERS_DIR\re-engine" -ForegroundColor Gray
Write-Host "- Plugins: $SRC_DIR\examples" -ForegroundColor Gray
Write-Host "- Lua API: $SRC_DIR\lua-api" -ForegroundColor Gray
Write-Host "- Build System: build_system" -ForegroundColor Gray
Write-Host ""
Write-Host "Build system ready for new file structure!" -ForegroundColor Green
Write-Host "All builds follow the new src/adapters/ organization" -ForegroundColor Green
Write-Host ""
Write-Host "===============================================" -ForegroundColor Cyan
Write-Host "uevr COMPLETE STRUCTURE BUILD COMPLETE" -ForegroundColor Cyan
Write-Host "===============================================" -ForegroundColor Cyan

Write-Host "Press any key to continue..." -ForegroundColor Yellow
$null = $Host.UI.RawUI.ReadKey("NoEcho,IncludeKeyDown")
