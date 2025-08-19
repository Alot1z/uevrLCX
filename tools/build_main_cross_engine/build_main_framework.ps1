# uevr Main Cross-Engine Framework Build Script
# Builds the core cross-engine system and registry
# Copyright (c) 2024 uevr Cross-Engine Adapter Project

param(
    [string]$BuildType = "Release",
    [switch]$WarningsAsErrors = $true,
    [int]$ParallelJobs = 8
)

# Set error action preference
$ErrorActionPreference = "Stop"

# Script configuration
$ScriptName = "build_main_framework.ps1"
$ProjectName = "uevr Main Cross-Engine Framework"
$SourceDir = "../../include"
$BuildDir = "build"
$LogsDir = "../../logs/build/main_cross_engine"
$CMakeFlags = "/W4"

if ($WarningsAsErrors) {
    $CMakeFlags += " /WX"
}

# Create logs directory
if (!(Test-Path $LogsDir)) {
    New-Item -ItemType Directory -Path $LogsDir -Force | Out-Null
}

# Logging function
function Write-Log {
    param([string]$Message, [string]$Level = "INFO")
    $timestamp = Get-Date -Format "yyyy-MM-dd HH:mm:ss"
    $logMessage = "[$timestamp] [$Level] $Message"
    Write-Host $logMessage
    Add-Content -Path "$LogsDir/build.log" -Value $logMessage
}

# Error handling function
function Write-ErrorLog {
    param([string]$Message, [string]$ErrorDetails = "")
    Write-Log $Message "ERROR"
    if ($ErrorDetails) {
        Write-Log "Error Details: $ErrorDetails" "ERROR"
        Add-Content -Path "$LogsDir/error.log" -Value "$(Get-Date -Format 'yyyy-MM-dd HH:mm:ss'): $Message`n$ErrorDetails`n"
    }
}

try {
    Write-Log "=== Starting $ProjectName Build ==="
    Write-Log "Build Type: $BuildType"
    Write-Log "Warnings as Errors: $WarningsAsErrors"
    Write-Log "Parallel Jobs: $ParallelJobs"
    Write-Log "CMake Flags: $CMakeFlags"
    Write-Log "Source Directory: $SourceDir"
    Write-Log "Build Directory: $BuildDir"
    Write-Log "Logs Directory: $LogsDir"
    
    # Check if source directory exists
    if (!(Test-Path $SourceDir)) {
        throw "Source directory not found: $SourceDir"
    }
    
    # Navigate to source directory
    Push-Location $SourceDir
    Write-Log "Changed to directory: $(Get-Location)"
    
    # Create build directory
    if (!(Test-Path $BuildDir)) {
        New-Item -ItemType Directory -Path $BuildDir -Force | Out-Null
        Write-Log "Created build directory: $BuildDir"
    }
    
    # Change to build directory
    Set-Location $BuildDir
    Write-Log "Changed to build directory: $(Get-Location)"
    
    # Configure with CMake
    Write-Log "Configuring with CMake..."
    $cmakeConfigArgs = @(
        "..",
        "-DCMAKE_BUILD_TYPE=$BuildType",
        "-DCMAKE_CXX_FLAGS=`"$CMakeFlags`"",
        "-G", "Visual Studio 17 2022",
        "-A", "x64"
    )
    
    $cmakeConfigOutput = & cmake @cmakeConfigArgs 2>&1
    if ($LASTEXITCODE -ne 0) {
        throw "CMake configuration failed with exit code $LASTEXITCODE`nOutput: $cmakeConfigOutput"
    }
    Write-Log "CMake configuration completed successfully"
    
    # Log CMake configuration output
    Add-Content -Path "$LogsDir/cmake_config.log" -Value $cmakeConfigOutput
    
    # Build the project
    Write-Log "Building project..."
    $cmakeBuildArgs = @(
        "--build", ".",
        "--config", $BuildType,
        "--parallel", $ParallelJobs
    )
    
    $cmakeBuildOutput = & cmake @cmakeBuildArgs 2>&1
    if ($LASTEXITCODE -ne 0) {
        throw "CMake build failed with exit code $LASTEXITCODE`nOutput: $cmakeBuildOutput"
    }
    Write-Log "CMake build completed successfully"
    
    # Log CMake build output
    Add-Content -Path "$LogsDir/cmake_build.log" -Value $cmakeBuildOutput
    
    # Verify build outputs
    Write-Log "Verifying build outputs..."
    $expectedOutputs = @(
        "bin\$BuildType\uevr_cross_engine_system.dll",
        "lib\$BuildType\uevr_cross_engine_system.lib"
    )
    
    $missingOutputs = @()
    foreach ($output in $expectedOutputs) {
        if (Test-Path $output) {
            Write-Log "✓ Found: $output"
        } else {
            Write-Log "✗ Missing: $output" "WARNING"
            $missingOutputs += $output
        }
    }
    
    if ($missingOutputs.Count -gt 0) {
        Write-Log "Warning: Some expected outputs are missing" "WARNING"
        Add-Content -Path "$LogsDir/missing_outputs.log" -Value "Missing outputs: $($missingOutputs -join ', ')"
    }
    
    # Build summary
    Write-Log "=== Build Summary ==="
    Write-Log "Project: $ProjectName"
    Write-Log "Status: SUCCESS"
    Write-Log "Build Type: $BuildType"
    Write-Log "Warnings as Errors: $WarningsAsErrors"
    Write-Log "Logs saved to: $LogsDir"
    
    # Create build success marker
    Add-Content -Path "$LogsDir/build_success.txt" -Value "Build completed successfully at $(Get-Date -Format 'yyyy-MM-dd HH:mm:ss')"
    
} catch {
    Write-ErrorLog "Build failed: $($_.Exception.Message)" $_.Exception.ToString()
    Write-Log "=== Build Summary ===" "ERROR"
    Write-Log "Project: $ProjectName" "ERROR"
    Write-Log "Status: FAILED" "ERROR"
    Write-Log "Error: $($_.Exception.Message)" "ERROR"
    
    # Create build failure marker
    Add-Content -Path "$LogsDir/build_failed.txt" -Value "Build failed at $(Get-Date -Format 'yyyy-MM-dd HH:mm:ss'): $($_.Exception.Message)"
    
    exit 1
} finally {
    # Restore original location
    Pop-Location
    Write-Log "Restored original location: $(Get-Location)"
}
