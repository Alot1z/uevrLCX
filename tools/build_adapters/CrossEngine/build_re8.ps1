# RE8 Cross-Engine VR Adapter Build Script
# Builds RE8 support for uevr Cross-Engine system

Write-Host ""
Write-Host "========================================" -ForegroundColor Green
Write-Host "RE8 Cross-Engine VR Adapter Build" -ForegroundColor Green
Write-Host "========================================" -ForegroundColor Green
Write-Host ""

# Set build environment
$BuildDir = "build_re8"
$SourceDir = "src\adapters"
$OutputDir = "install_re8"

# Create build directory
if (!(Test-Path $BuildDir)) {
    New-Item -ItemType Directory -Path $BuildDir | Out-Null
}
Set-Location $BuildDir

# Configure CMake for RE8
Write-Host "Configuring CMake for RE8..." -ForegroundColor Yellow
cmake ..\..\$SourceDir -G "Visual Studio 17 2022" -A x64 `
    -Duevr_BUILD_RE8_ADAPTER=ON `
    -Duevr_BUILD_CROSS_ENGINE=ON `
    -Duevr_BUILD_ALL_ADAPTERS=ON `
    -DCMAKE_BUILD_TYPE=Release

if ($LASTEXITCODE -ne 0) {
    Write-Host "ERROR: CMake configuration failed!" -ForegroundColor Red
    Read-Host "Press Enter to continue..."
    exit 1
}

# Build RE8 adapter
Write-Host "Building RE8 adapter..." -ForegroundColor Yellow
cmake --build . --config Release --target uevr_re_engine_adapter

if ($LASTEXITCODE -ne 0) {
    Write-Host "ERROR: Build failed!" -ForegroundColor Red
    Read-Host "Press Enter to continue..."
    exit 1
}

# Build all cross-engine adapters
Write-Host "Building all cross-engine adapters..." -ForegroundColor Yellow
cmake --build . --config Release --target uevr_redengine4_adapter
cmake --build . --config Release --target uevr_mt_framework_adapter

# Create output directory
if (!(Test-Path "..\..\$OutputDir")) {
    New-Item -ItemType Directory -Path "..\..\$OutputDir" | Out-Null
}

# Copy built adapters
Write-Host "Copying built adapters..." -ForegroundColor Yellow
Copy-Item "Release\*.dll" "..\..\$OutputDir\" -Force
Copy-Item "Release\*.lib" "..\..\$OutputDir\" -Force

# Copy RE8 profile
Write-Host "Copying RE8 profile..." -ForegroundColor Yellow
Copy-Item "..\..\profiles\re8-Win64-Shipping" "..\..\$OutputDir\profiles\" -Recurse -Force

# Copy documentation
Write-Host "Copying documentation..." -ForegroundColor Yellow
Copy-Item "..\..\docs\cross-engine\*.md" "..\..\$OutputDir\docs\" -Force

Write-Host ""
Write-Host "========================================" -ForegroundColor Green
Write-Host "RE8 Build Complete!" -ForegroundColor Green
Write-Host "========================================" -ForegroundColor Green
Write-Host ""
Write-Host "Built adapters:" -ForegroundColor Cyan
Write-Host "- RE Engine Adapter (RE7 + RE8)" -ForegroundColor White
Write-Host "- REDengine 4 Adapter (Cyberpunk 2077)" -ForegroundColor White
Write-Host "- MT Framework Adapter (Monster Hunter World)" -ForegroundColor White
Write-Host ""
Write-Host "Output directory: $OutputDir" -ForegroundColor Cyan
Write-Host ""

Read-Host "Press Enter to continue..."
