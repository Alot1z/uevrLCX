# ========================================
# UEVR Restructured Project Build Script
# ========================================
# This script builds the complete UEVR project with all components
# including core framework, adapters, hooks, mods, tests, and examples
# 
# Usage: .\build_restructured.ps1 [Configuration]
# Valid configurations: Debug, Release, RelWithDebInfo, MinSizeRel
# Default: Release

param(
    [Parameter(Position=0)]
    [ValidateSet("Debug", "Release", "RelWithDebInfo", "MinSizeRel")]
    [string]$Configuration = "Release"
)

# Set execution policy for this session
Set-ExecutionPolicy -ExecutionPolicy Bypass -Scope Process -Force

# Function to write colored output
function Write-ColorOutput {
    param(
        [string]$Message,
        [string]$Color = "White"
    )
    Write-Host $Message -ForegroundColor $Color
}

# Function to check if command exists
function Test-Command {
    param([string]$Command)
    try {
        Get-Command $Command -ErrorAction Stop | Out-Null
        return $true
    } catch {
        return $false
    }
}

# Function to get timestamp
function Get-Timestamp {
    return Get-Date -Format "yyyyMMdd_HHmmss"
}

# Main execution
try {
    Write-ColorOutput "========================================" "Cyan"
    Write-ColorOutput "    UEVR Restructured Project Builder" "Cyan"
    Write-ColorOutput "========================================" "Cyan"
    Write-Host ""

    # Check prerequisites
    Write-ColorOutput "Checking prerequisites..." "Yellow"
    
    if (-not (Test-Command "cmake")) {
        throw "CMake is not installed or not in PATH! Please install CMake from: https://cmake.org/download/"
    }
    
    if (-not (Test-Command "cl")) {
        Write-ColorOutput "WARNING: Visual Studio compiler not found in PATH" "Yellow"
        Write-ColorOutput "This may cause build issues. Please run from Developer Command Prompt." "Yellow"
        Write-Host ""
    }
    
    Write-ColorOutput "Prerequisites check completed." "Green"
    Write-Host ""

    # Display build information
    Write-ColorOutput "Build Information:" "Cyan"
    Write-ColorOutput "  Configuration: $Configuration" "White"
    Write-ColorOutput "  CMake Version: $(cmake --version | Select-Object -First 1)" "White"
    Write-ColorOutput "  PowerShell Version: $($PSVersionTable.PSVersion)" "White"
    Write-ColorOutput "  Working Directory: $(Get-Location)" "White"
    Write-Host ""

    # Create build directory with timestamp
    $timestamp = Get-Timestamp
    $buildDir = "build_$timestamp"
    Write-ColorOutput "Creating build directory: $buildDir" "Yellow"
    
    if (-not (Test-Path $buildDir)) {
        New-Item -ItemType Directory -Name $buildDir -Force | Out-Null
    }
    Set-Location $buildDir

    # Configure with CMake
    Write-ColorOutput "========================================" "Cyan"
    Write-ColorOutput "Configuring project with CMake..." "Cyan"
    Write-ColorOutput "========================================" "Cyan"
    Write-Host ""

    $cmakeOptions = @(
        "-G", "Visual Studio 17 2022",
        "-A", "x64",
        "-DCMAKE_BUILD_TYPE=$Configuration",
        "-DBUILD_TESTS=ON",
        "-DBUILD_EXAMPLES=ON",
        "-DUEVR_ENABLE_DEBUG=OFF",
        "-DUEVR_ENABLE_PROFILING=OFF"
    )

    Write-ColorOutput "CMake Options: $($cmakeOptions -join ' ')" "White"
    Write-Host ""

    $cmakeProcess = Start-Process -FilePath "cmake" -ArgumentList $cmakeOptions -WorkingDirectory ".." -Wait -PassThru -NoNewWindow
    
    if ($cmakeProcess.ExitCode -ne 0) {
        throw "CMake configuration failed with exit code $($cmakeProcess.ExitCode)"
    }

    Write-ColorOutput "========================================" "Cyan"
    Write-ColorOutput "CMake configuration successful!" "Cyan"
    Write-ColorOutput "========================================" "Cyan"
    Write-Host ""

    # Build the project
    Write-ColorOutput "========================================" "Cyan"
    Write-ColorOutput "Building project..." "Cyan"
    Write-ColorOutput "========================================" "Cyan"
    Write-Host ""

    $buildOptions = @(
        "--build", ".",
        "--config", $Configuration,
        "--parallel"
    )

    Write-ColorOutput "Build Options: $($buildOptions -join ' ')" "White"
    Write-Host ""

    $buildProcess = Start-Process -FilePath "cmake" -ArgumentList $buildOptions -Wait -PassThru -NoNewWindow
    
    if ($buildProcess.ExitCode -ne 0) {
        throw "Build failed with exit code $($buildProcess.ExitCode)"
    }

    Write-ColorOutput "========================================" "Cyan"
    Write-ColorOutput "Build completed successfully!" "Cyan"
    Write-ColorOutput "========================================" "Cyan"
    Write-Host ""

    # Build summary
    Write-ColorOutput "Build Summary:" "Green"
    Write-ColorOutput "  Configuration: $Configuration" "White"
    Write-ColorOutput "  Build Directory: $buildDir" "White"
    Write-ColorOutput "  Output Files: $buildDir\bin\$Configuration\" "White"
    Write-ColorOutput "  Libraries: $buildDir\lib\$Configuration\" "White"
    Write-Host ""

    # List output files
    $outputDir = "bin\$Configuration"
    if (Test-Path $outputDir) {
        Write-ColorOutput "Generated Files:" "Yellow"
        Get-ChildItem $outputDir | ForEach-Object {
            Write-ColorOutput "  $($_.Name)" "White"
        }
        Write-Host ""
    }

    # Run tests if available
    $testExe = "bin\$Configuration\uevr_tests.exe"
    if (Test-Path $testExe) {
        Write-ColorOutput "Running tests..." "Yellow"
        & $testExe
        Write-Host ""
    }

    Write-ColorOutput "========================================" "Cyan"
    Write-ColorOutput "Build process completed!" "Cyan"
    Write-ColorOutput "========================================" "Cyan"
    Write-Host ""

} catch {
    Write-ColorOutput "========================================" "Red"
    Write-ColorOutput "ERROR: $($_.Exception.Message)" "Red"
    Write-ColorOutput "========================================" "Red"
    Write-Host ""
    
    Write-ColorOutput "Common issues:" "Yellow"
    Write-ColorOutput "  - Missing dependencies (MinHook, spdlog, nlohmann_json)" "White"
    Write-ColorOutput "  - CMake version too old (requires 3.16+)" "White"
    Write-ColorOutput "  - Visual Studio not properly installed" "White"
    Write-ColorOutput "  - Compilation errors in source files" "White"
    Write-ColorOutput "  - Missing include paths" "White"
    Write-ColorOutput "  - Linker errors with dependencies" "White"
    Write-ColorOutput "  - Insufficient disk space" "White"
    Write-Host ""
    
    exit 1
} finally {
    # Return to original directory
    if (Test-Path "..") {
        Set-Location ".."
    }
}

Read-Host "Press Enter to continue..."
