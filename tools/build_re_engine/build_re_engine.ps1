# uevr RE Engine Adapter Build Script
# Builds Resident Evil 7 VR adapter with full cross-engine integration
# Copyright (c) 2024 uevr Cross-Engine Adapter Project

param(
    [string]$BuildType = "Release",
    [switch]$WarningsAsErrors = $true,
    [int]$ParallelJobs = 8
)

# Set error action preference
$ErrorActionPreference = "Stop"

# Script configuration
$ScriptName = "build_re_engine.ps1"
$ProjectName = "uevr RE Engine Adapter"
$SourceDir = "../../adapters/RE-Engine"
$BuildDir = "build"
$LogsDir = "../../logs/build/re_engine"
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
        "re_engine_adapter.dll",
        "re_engine_adapter.lib"
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
    
    # Deploy to profile directory
    Write-Log "Deploying to Resident Evil 7 profile..."
    $profileDir = "../../profiles/re7"
    $dllPath = "re_engine_adapter.dll"
    
    if (Test-Path $dllPath) {
        if (!(Test-Path $profileDir)) {
            New-Item -ItemType Directory -Path $profileDir -Force | Out-Null
            Write-Log "Created profile directory: $profileDir"
        }
        
        Copy-Item -Path $dllPath -Destination $profileDir -Force
        Write-Log "✓ Deployed $dllPath to $profileDir"
        
        # Verify deployment
        if (Test-Path "$profileDir/$dllPath") {
            Write-Log "✓ Deployment verification successful"
        } else {
            Write-Log "✗ Deployment verification failed" "WARNING"
        }
    } else {
        Write-Log "✗ Cannot deploy: $dllPath not found" "WARNING"
    }
    
    # Build summary
    Write-Log "=== Build Summary ==="
    Write-Log "Project: $ProjectName"
    Write-Log "Status: SUCCESS"
    Write-Log "Build Type: $BuildType"
    Write-Log "Warnings as Errors: $WarningsAsErrors"
    Write-Log "Logs saved to: $LogsDir"
    Write-Log "Deployed to: $profileDir"
    
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
