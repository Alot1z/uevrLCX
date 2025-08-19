# ========================================
# uevr System Test Suite
# ========================================
# This script provides comprehensive testing of the uevr system
# including build validation, component testing, and integration testing

param(
    [string]$BuildType = "Release",
    [string]$Architecture = "x64",
    [switch]$SkipBuild,
    [switch]$RunTests,
    [switch]$CleanBuild,
    [switch]$Verbose
)

# Set error action preference
$ErrorActionPreference = "Stop"

# Color functions for output
function Write-ColorOutput {
    param(
        [string]$Message,
        [string]$Color = "White"
    )
    Write-Host $Message -ForegroundColor $Color
}

function Write-Success {
    param([string]$Message)
    Write-ColorOutput "✅ $Message" "Green"
}

function Write-Error {
    param([string]$Message)
    Write-ColorOutput "❌ $Message" "Red"
}

function Write-Warning {
    param([string]$Message)
    Write-ColorOutput "⚠️  $Message" "Yellow"
}

function Write-Info {
    param([string]$Message)
    Write-ColorOutput "ℹ️  $Message" "Cyan"
}

# Test configuration
$TestConfig = @{
    BuildType = $BuildType
    Architecture = $Architecture
    BuildDir = "build_${Architecture}_${BuildType}"
    InstallDir = "install_${Architecture}_${BuildType}"
    TestResultsDir = "test_results"
    LogFile = "uevr_test_$(Get-Date -Format 'yyyyMMdd_HHmmss').log"
}

# Initialize logging
function Start-Logging {
    param([string]$LogFile)
    
    if (!(Test-Path $TestConfig.TestResultsDir)) {
        New-Item -ItemType Directory -Path $TestConfig.TestResultsDir -Force | Out-Null
    }
    
    Start-Transcript -Path "$($TestConfig.TestResultsDir)\$LogFile" -Append
    Write-Info "uevr System Test Suite started at $(Get-Date)"
    Write-Info "Build Type: $($TestConfig.BuildType)"
    Write-Info "Architecture: $($TestConfig.Architecture)"
    Write-Info "Log File: $LogFile"
    Write-Host ""
}

function Stop-Logging {
    Write-Info "uevr System Test Suite completed at $(Get-Date)"
    Stop-Transcript
}

# Check system requirements
function Test-SystemRequirements {
    Write-Info "Checking system requirements..."
    
    $requirements = @{
        "CMake" = $false
        "Visual Studio" = $false
        "PowerShell 5.1+" = $false
        "Windows 10+" = $false
    }
    
    # Check CMake
    try {
        $cmakeVersion = cmake --version 2>&1
        if ($cmakeVersion -match "cmake version") {
            $requirements["CMake"] = $true
            Write-Success "CMake found: $($cmakeVersion.Split("`n")[0])"
        }
    } catch {
        Write-Warning "CMake not found in PATH"
    }
    
    # Check Visual Studio
    try {
        $clVersion = cl 2>&1
        if ($clVersion -match "Microsoft") {
            $requirements["Visual Studio"] = $true
            Write-Success "Visual Studio compiler found"
        }
    } catch {
        Write-Warning "Visual Studio compiler not found - run from Developer Command Prompt"
    }
    
    # Check PowerShell version
    if ($PSVersionTable.PSVersion.Major -ge 5) {
        $requirements["PowerShell 5.1+"] = $true
        Write-Success "PowerShell version: $($PSVersionTable.PSVersion)"
    } else {
        Write-Warning "PowerShell version too old: $($PSVersionTable.PSVersion)"
    }
    
    # Check Windows version
    $osInfo = Get-CimInstance -ClassName Win32_OperatingSystem
    if ([version]$osInfo.Version -ge [version]"10.0") {
        $requirements["Windows 10+"] = $true
        Write-Success "Windows version: $($osInfo.Caption) $($osInfo.Version)"
    } else {
        Write-Warning "Windows version too old: $($osInfo.Caption) $($osInfo.Version)"
    }
    
    # Summary
    $passed = ($requirements.Values | Where-Object { $_ -eq $true }).Count
    $total = $requirements.Count
    
    Write-Host ""
    Write-Info "System Requirements: $passed/$total passed"
    
    if ($passed -eq $total) {
        Write-Success "All system requirements met!"
        return $true
    } else {
        Write-Warning "Some system requirements not met. Tests may fail."
        return $false
    }
}

# Clean build directory
function Clean-BuildDirectory {
    if ($CleanBuild -and (Test-Path $TestConfig.BuildDir)) {
        Write-Info "Cleaning build directory: $($TestConfig.BuildDir)"
        Remove-Item -Path $TestConfig.BuildDir -Recurse -Force
        Write-Success "Build directory cleaned"
    }
}

# Build the project
function Build-Project {
    if ($SkipBuild) {
        Write-Info "Skipping build as requested"
        return $true
    }
    
    Write-Info "Building uevr project..."
    
    # Create build directory
    if (!(Test-Path $TestConfig.BuildDir)) {
        New-Item -ItemType Directory -Path $TestConfig.BuildDir -Force | Out-Null
    }
    
    # Navigate to build directory
    Push-Location $TestConfig.BuildDir
    
    try {
        # Configure with CMake
        Write-Info "Configuring project with CMake..."
        $cmakeArgs = @(
            "..",
            "-DCMAKE_BUILD_TYPE=$($TestConfig.BuildType)",
            "-DCMAKE_INSTALL_PREFIX=../$($TestConfig.InstallDir)",
            "-DCMAKE_GENERATOR_PLATFORM=$($TestConfig.Architecture)",
            "-DENABLE_VR=ON",
            "-DENABLE_DEBUG=ON"
        )
        
        if ($Verbose) {
            $cmakeArgs += "-DCMAKE_VERBOSE_MAKEFILE=ON"
        }
        
        $cmakeResult = & cmake @cmakeArgs 2>&1
        if ($LASTEXITCODE -ne 0) {
            throw "CMake configuration failed: $cmakeResult"
        }
        Write-Success "CMake configuration completed"
        
        # Build project
        Write-Info "Building project..."
        $buildArgs = @(
            "--build", ".",
            "--config", $TestConfig.BuildType,
            "--parallel"
        )
        
        if ($Verbose) {
            $buildArgs += "--verbose"
        }
        
        $buildResult = & cmake @buildArgs 2>&1
        if ($LASTEXITCODE -ne 0) {
            throw "Build failed: $buildResult"
        }
        Write-Success "Build completed successfully"
        
        # Install project
        Write-Info "Installing project..."
        $installResult = & cmake --install . --config $TestConfig.BuildType 2>&1
        if ($LASTEXITCODE -ne 0) {
            throw "Installation failed: $installResult"
        }
        Write-Success "Installation completed"
        
        return $true
        
    } catch {
        Write-Error "Build failed: $_"
        return $false
    } finally {
        Pop-Location
    }
}

# Test individual components
function Test-IndividualComponents {
    Write-Info "Testing individual components..."
    
    $componentTests = @{
        "RE Engine Adapter" = { Test-REEngineAdapter }
        "UObjectHook System" = { Test-UObjectHookSystem }
        "Build System" = { Test-BuildSystem }
        "Configuration Files" = { Test-ConfigurationFiles }
    }
    
    $results = @{}
    $passed = 0
    $total = $componentTests.Count
    
    foreach ($component in $componentTests.Keys) {
        Write-Info "Testing component: $component"
        try {
            $result = & $componentTests[$component]
            $results[$component] = $result
            if ($result) {
                Write-Success "$component test passed"
                $passed++
            } else {
                Write-Error "$component test failed"
            }
        } catch {
            Write-Error "$component test exception: $_"
            $results[$component] = $false
        }
        Write-Host ""
    }
    
    Write-Info "Component Tests: $passed/$total passed"
    return $results
}

function Test-REEngineAdapter {
    # Test RE Engine adapter functionality
    try {
        # Check if header file exists
        if (!(Test-Path "src/adapters/re-engine/REEngineAdapter.hpp")) {
            Write-Warning "RE Engine adapter header not found"
            return $false
        }
        
        # Check if implementation file exists
        if (!(Test-Path "src/adapters/re-engine/REEngineAdapter.cpp")) {
            Write-Warning "RE Engine adapter implementation not found"
            return $false
        }
        
        # Check for required includes
        $headerContent = Get-Content "src/adapters/re-engine/REEngineAdapter.hpp" -Raw
        if ($headerContent -match "MinHook" -and $headerContent -match "D3D11") {
            Write-Success "RE Engine adapter includes required dependencies"
        } else {
            Write-Warning "RE Engine adapter missing required dependencies"
            return $false
        }
        
        return $true
        
    } catch {
        Write-Error "RE Engine adapter test exception: $_"
        return $false
    }
}

function Test-UObjectHookSystem {
    # Test UObjectHook system functionality
    try {
        # Check if header file exists
        if (!(Test-Path "src/adapters/common/UObjectHookSystem.hpp")) {
            Write-Warning "UObjectHook system header not found"
            return $false
        }
        
        # Check if implementation file exists
        if (!(Test-Path "src/adapters/common/UObjectHookSystem.cpp")) {
            Write-Warning "UObjectHook system implementation not found"
            return $false
        }
        
        # Check for required functionality
        $headerContent = Get-Content "src/adapters/common/UObjectHookSystem.hpp" -Raw
        if ($headerContent -match "ComponentAttachment" -and $headerContent -match "scanForComponents") {
            Write-Success "UObjectHook system has required functionality"
        } else {
            Write-Warning "UObjectHook system missing required functionality"
            return $false
        }
        
        return $true
        
    } catch {
        Write-Error "UObjectHook system test exception: $_"
        return $false
    }
}

function Test-BuildSystem {
    # Test build system functionality
    try {
        # Check if build script exists
        if (!(Test-Path "build_restructured.bat")) {
            Write-Warning "Build script not found"
            return $false
        }
        
        # Check if CMakeLists.txt exists
        if (!(Test-Path "CMakeLists.txt")) {
            Write-Warning "CMakeLists.txt not found"
            return $false
        }
        
        # Check build script content
        $buildScriptContent = Get-Content "build_restructured.bat" -Raw
        if ($buildScriptContent -match "CMake" -and $buildScriptContent -match "VR") {
            Write-Success "Build script has required functionality"
        } else {
            Write-Warning "Build script missing required functionality"
            return $false
        }
        
        return $true
        
    } catch {
        Write-Error "Build system test exception: $_"
        return $false
    }
}

function Test-ConfigurationFiles {
    # Test configuration files
    try {
        $configFiles = @(
            "config",
            "profiles",
            "docs"
        )
        
        $found = 0
        foreach ($dir in $configFiles) {
            if (Test-Path $dir) {
                $found++
                Write-Success "Configuration directory found: $dir"
            } else {
                Write-Warning "Configuration directory missing: $dir"
            }
        }
        
        if ($found -eq $configFiles.Count) {
            Write-Success "All configuration directories found"
            return $true
        } else {
            Write-Warning "Some configuration directories missing"
            return $false
        }
        
    } catch {
        Write-Error "Configuration files test exception: $_"
        return $false
    }
}

# Run integration tests
function Test-Integration {
    Write-Info "Running integration tests..."
    
    try {
        # Test if build artifacts exist
        $buildArtifacts = @(
            "$($TestConfig.InstallDir)/bin",
            "$($TestConfig.InstallDir)/config",
            "$($TestConfig.InstallDir)/profiles"
        )
        
        $found = 0
        foreach ($artifact in $buildArtifacts) {
            if (Test-Path $artifact) {
                $found++
                Write-Success "Build artifact found: $artifact"
            } else {
                Write-Warning "Build artifact missing: $artifact"
            }
        }
        
        if ($found -eq $buildArtifacts.Count) {
            Write-Success "All build artifacts found"
            return $true
        } else {
            Write-Warning "Some build artifacts missing"
            return $false
        }
        
    } catch {
        Write-Error "Integration test exception: $_"
        return $false
    }
}

# Generate test report
function Generate-TestReport {
    param($ComponentResults, $IntegrationResult)
    
    Write-Info "Generating test report..."
    
    $reportFile = "$($TestConfig.TestResultsDir)/test_report_$(Get-Date -Format 'yyyyMMdd_HHmmss').md"
    
    $report = @"
# uevr System Test Report

**Generated:** $(Get-Date)
**Build Type:** $($TestConfig.BuildType)
**Architecture:** $($TestConfig.Architecture)

## Test Summary

### Component Tests
"@
    
    foreach ($component in $ComponentResults.Keys) {
        $status = if ($ComponentResults[$component]) { "✅ PASSED" } else { "❌ FAILED" }
        $report += "`n- **$component**: $status"
    }
    
    $report += @"

### Integration Tests
- **Build Integration**: $(if ($IntegrationResult) { "✅ PASSED" } else { "❌ FAILED" })

## System Requirements
- **CMake**: $(if (Get-Command cmake -ErrorAction SilentlyContinue) { "✅ Found" } else { "❌ Not Found" })
- **Visual Studio**: $(if (Get-Command cl -ErrorAction SilentlyContinue) { "✅ Found" } else { "❌ Not Found" })
- **PowerShell**: ✅ $($PSVersionTable.PSVersion)
- **Windows**: ✅ $((Get-CimInstance -ClassName Win32_OperatingSystem).Caption)

## Recommendations
"@
    
    if ($ComponentResults.Values -contains $false) {
        $report += "`n- Review failed component tests"
    }
    
    if (!$IntegrationResult) {
        $report += "`n- Check build system configuration"
    }
    
    if (!(Get-Command cmake -ErrorAction SilentlyContinue)) {
        $report += "`n- Install CMake and add to PATH"
    }
    
    if (!(Get-Command cl -ErrorAction SilentlyContinue)) {
        $report += "`n- Run from Visual Studio Developer Command Prompt"
    }
    
    $report += "`n`n---`n*Report generated by uevr System Test Suite*"
    
    # Save report
    $report | Out-File -FilePath $reportFile -Encoding UTF8
    Write-Success "Test report saved to: $reportFile"
    
    return $reportFile
}

# Main test execution
function Main {
    try {
        # Start logging
        Start-Logging -LogFile $TestConfig.LogFile
        
        Write-ColorOutput "========================================" "Magenta"
        Write-ColorOutput "uevr System Test Suite" "Magenta"
        Write-ColorOutput "========================================" "Magenta"
        Write-Host ""
        
        # Check system requirements
        $requirementsMet = Test-SystemRequirements
        Write-Host ""
        
        # Clean build directory if requested
        Clean-BuildDirectory
        Write-Host ""
        
        # Build project
        $buildSuccess = Build-Project
        Write-Host ""
        
        if (!$buildSuccess) {
            Write-Error "Build failed - cannot continue with tests"
            return 1
        }
        
        # Test individual components
        $componentResults = Test-IndividualComponents
        Write-Host ""
        
        # Run integration tests
        $integrationResult = Test-Integration
        Write-Host ""
        
        # Generate test report
        $reportFile = Generate-TestReport -ComponentResults $componentResults -IntegrationResult $integrationResult
        Write-Host ""
        
        # Summary
        $componentPassed = ($componentResults.Values | Where-Object { $_ -eq $true }).Count
        $componentTotal = $componentResults.Count
        
        Write-ColorOutput "========================================" "Magenta"
        Write-ColorOutput "TEST SUMMARY" "Magenta"
        Write-ColorOutput "========================================" "Magenta"
        Write-Info "Component Tests: $componentPassed/$componentTotal passed"
        Write-Info "Integration Tests: $(if ($integrationResult) { "PASSED" } else { "FAILED" })"
        Write-Info "System Requirements: $(if ($requirementsMet) { "MET" } else { "NOT MET" })"
        Write-Info "Test Report: $reportFile"
        
        if ($componentPassed -eq $componentTotal -and $integrationResult -and $requirementsMet) {
            Write-Success "ALL TESTS PASSED! uevr system is ready for use."
            return 0
        } else {
            Write-Warning "Some tests failed. Please review the test report."
            return 1
        }
        
    } catch {
        Write-Error "Test suite execution failed: $_"
        return 1
    } finally {
        Stop-Logging
    }
}

# Execute main function
if ($MyInvocation.InvocationName -ne ".") {
    Main
}
