# uevr Cross-Engine VR Adapters - Build Validator
# Validates all builds and generates comprehensive QA reports
# Copyright (c) 2024 uevr Cross-Engine Adapter Project

param(
    [string]$ValidationMode = "comprehensive", # comprehensive, quick, specific
    [string]$TargetComponent = "all", # all, main, mt, re, red
    [switch]$GenerateQAReport = $true,
    [switch]$ValidateDeployment = $true,
    [switch]$CheckDependencies = $true,
    [switch]$PerformanceValidation = $false
)

# Set error action preference
$ErrorActionPreference = "Continue"

# Script configuration
$ScriptName = "build_validator.ps1"
$ToolName = "uevr Build Validator"
$BuildToolsDir = "../"
$ReportsDir = "../../logs/validation"
$LogsDir = "../../logs/build"
$ProfilesDir = "../../profiles"

# Create directories
if (!(Test-Path $ReportsDir)) {
    New-Item -ItemType Directory -Path $ReportsDir -Force | Out-Null
}

# Logging function
function Write-Log {
    param([string]$Message, [string]$Level = "INFO")
    $timestamp = Get-Date -Format "yyyy-MM-dd HH:mm:ss"
    $logMessage = "[$timestamp] [$Level] $Message"
    Write-Host $logMessage
    Add-Content -Path "$ReportsDir/validation.log" -Value $logMessage
}

# Validation results storage
$ValidationResults = @{
    MainFramework = @{}
    MTFramework = @{}
    REEngine = @{}
    REDengine4 = @{}
    Overall = @{}
}

# Component validation function
function Validate-Component {
    param(
        [string]$ComponentName,
        [string]$ComponentPath,
        [string]$LogPath,
        [string]$ProfilePath
    )
    
    Write-Log "=== Validating $ComponentName ==="
    
    $validationResults = @{
        ComponentName = $ComponentName
        Status = "Unknown"
        BuildValidation = @{}
        DeploymentValidation = @{}
        DependencyValidation = @{}
        PerformanceValidation = @{}
        Issues = @()
        Warnings = @()
        PassedChecks = 0
        TotalChecks = 0
        ValidationScore = 0
    }
    
    # 1. Build Validation
    Write-Log "Validating build process..."
    $buildValidation = @{
        BuildLogsExist = $false
        BuildSuccess = $false
        BuildTime = $null
        SuccessRate = 0
        ErrorCount = 0
        WarningCount = 0
    }
    
    if (Test-Path $LogPath) {
        $buildValidation.BuildLogsExist = $true
        
        # Check build success/failure
        if (Test-Path "$LogPath/build_success.txt") {
            $buildValidation.BuildSuccess = $true
            $buildValidation.Status = "Success"
        } elseif (Test-Path "$LogPath/build_failed.txt") {
            $buildValidation.BuildSuccess = $false
            $buildValidation.Status = "Failed"
        }
        
        # Analyze build logs
        if (Test-Path "$LogPath/build.log") {
            $buildLog = Get-Content "$LogPath/build.log"
            
            # Count successful vs failed builds
            $successCount = ($buildLog | Select-String "SUCCESS").Count
            $failedCount = ($buildLog | Select-String "FAILED").Count
            $totalCount = $successCount + $failedCount
            
            if ($totalCount -gt 0) {
                $buildValidation.SuccessRate = [math]::Round(($successCount / $totalCount) * 100, 1)
            }
            
            # Count errors and warnings
            $buildValidation.ErrorCount = ($buildLog | Select-String "ERROR").Count
            $buildValidation.WarningCount = ($buildLog | Select-String "WARNING").Count
            
            # Extract build time
            $buildTimeLines = $buildLog | Select-String "Duration:"
            if ($buildTimeLines) {
                $lastBuildTime = $buildTimeLines | Select-Object -Last 1
                $buildValidation.BuildTime = ($lastBuildTime.Line -split "Duration: ")[1]
            }
        }
        
        # Check CMake logs
        if (Test-Path "$LogPath/cmake_config.log") {
            $cmakeConfig = Get-Content "$LogPath/cmake_config.log"
            $configErrors = $cmakeConfig | Select-String "error|Error|ERROR|failed|Failed|FAILED"
            $buildValidation.ErrorCount += $configErrors.Count
        }
        
        if (Test-Path "$LogPath/cmake_build.log") {
            $cmakeBuild = Get-Content "$LogPath/cmake_build.log"
            $buildErrors = $cmakeBuild | Select-String "error|Error|ERROR|failed|Failed|FAILED"
            $buildValidation.ErrorCount += $buildErrors.Count
        }
    } else {
        $validationResults.Issues += "No build logs found for $ComponentName"
    }
    
    $validationResults.BuildValidation = $buildValidation
    
    # 2. Deployment Validation
    if ($ValidateDeployment) {
        Write-Log "Validating deployment..."
        $deploymentValidation = @{
            ProfileDirectoryExists = $false
            AdapterDLLExists = $false
            AdapterDLLSize = 0
            AdapterDLLVersion = $null
            DeploymentTime = $null
        }
        
        if (Test-Path $ProfilePath) {
            $deploymentValidation.ProfileDirectoryExists = $true
            
            # Check for adapter DLL
            $adapterDLLs = @{
                "MainFramework" = "uevr_cross_engine_system.dll"
                "MTFramework" = "mt_framework_cross_engine_adapter.dll"
                "REEngine" = "re_engine_adapter.dll"
                "REDengine4" = "redengine4_adapter.dll"
            }
            
            $expectedDLL = $adapterDLLs[$ComponentName]
            if ($expectedDLL -and (Test-Path "$ProfilePath/$expectedDLL")) {
                $deploymentValidation.AdapterDLLExists = $true
                
                # Get DLL information
                $dllPath = "$ProfilePath/$expectedDLL"
                $dllInfo = Get-Item $dllPath
                $deploymentValidation.AdapterDLLSize = $dllInfo.Length
                $deploymentValidation.DeploymentTime = $dllInfo.LastWriteTime
                
                # Check DLL version (if available)
                try {
                    $versionInfo = [System.Diagnostics.FileVersionInfo]::GetVersionInfo($dllPath)
                    $deploymentValidation.AdapterDLLVersion = $versionInfo.FileVersion
                } catch {
                    $deploymentValidation.AdapterDLLVersion = "Version info not available"
                }
            } else {
                $validationResults.Issues += "Expected adapter DLL not found: $expectedDLL"
            }
        } else {
            $validationResults.Issues += "Profile directory not found: $ProfilePath"
        }
        
        $validationResults.DeploymentValidation = $deploymentValidation
    }
    
    # 3. Dependency Validation
    if ($CheckDependencies) {
        Write-Log "Validating dependencies..."
        $dependencyValidation = @{
            CMakeAvailable = $false
            VisualStudioAvailable = $false
            PowerShellAvailable = $false
            RequiredLibraries = @()
            MissingDependencies = @()
        }
        
        # Check CMake
        try {
            $cmakeVersion = & cmake --version 2>&1
            if ($LASTEXITCODE -eq 0) {
                $dependencyValidation.CMakeAvailable = $true
            } else {
                $dependencyValidation.MissingDependencies += "CMake not available or not in PATH"
            }
        } catch {
            $dependencyValidation.MissingDependencies += "CMake not available or not in PATH"
        }
        
        # Check Visual Studio
        try {
            $clVersion = & cl 2>&1
            if ($LASTEXITCODE -eq 0) {
                $dependencyValidation.VisualStudioAvailable = $true
            } else {
                $dependencyValidation.MissingDependencies += "Visual Studio compiler not available"
            }
        } catch {
            $dependencyValidation.MissingDependencies += "Visual Studio compiler not available"
        }
        
        # Check PowerShell
        try {
            $psVersion = $PSVersionTable.PSVersion
            $dependencyValidation.PowerShellAvailable = $true
        } catch {
            $dependencyValidation.MissingDependencies += "PowerShell not available"
        }
        
        # Check for required libraries
        $requiredLibs = @("MinHook", "spdlog", "nlohmann/json")
        foreach ($lib in $requiredLibs) {
            $dependencyValidation.RequiredLibraries += $lib
        }
        
        $validationResults.DependencyValidation = $dependencyValidation
    }
    
    # 4. Performance Validation
    if ($PerformanceValidation) {
        Write-Log "Validating performance metrics..."
        $performanceValidation = @{
            BuildTime = $buildValidation.BuildTime
            SuccessRate = $buildValidation.SuccessRate
            ErrorRate = 0
            PerformanceScore = 0
        }
        
        if ($buildValidation.SuccessRate -gt 0) {
            $performanceValidation.ErrorRate = 100 - $buildValidation.SuccessRate
        }
        
        # Calculate performance score
        $performanceScore = 0
        if ($buildValidation.BuildSuccess) { $performanceScore += 40 }
        if ($buildValidation.SuccessRate -ge 90) { $performanceScore += 30 }
        if ($buildValidation.ErrorCount -eq 0) { $performanceScore += 20 }
        if ($buildValidation.WarningCount -le 5) { $performanceScore += 10 }
        
        $performanceValidation.PerformanceScore = $performanceScore
        $validationResults.PerformanceValidation = $performanceValidation
    }
    
    # Calculate overall validation score
    $passedChecks = 0
    $totalChecks = 0
    
    # Build validation checks
    if ($buildValidation.BuildLogsExist) { $totalChecks++; if ($buildValidation.BuildSuccess) { $passedChecks++ } }
    if ($buildValidation.SuccessRate -ge 90) { $totalChecks++; $passedChecks++ }
    if ($buildValidation.ErrorCount -eq 0) { $totalChecks++; $passedChecks++ }
    
    # Deployment validation checks
    if ($ValidateDeployment) {
        if ($deploymentValidation.ProfileDirectoryExists) { $totalChecks++; $passedChecks++ }
        if ($deploymentValidation.AdapterDLLExists) { $totalChecks++; $passedChecks++ }
    }
    
    # Dependency validation checks
    if ($CheckDependencies) {
        if ($dependencyValidation.CMakeAvailable) { $totalChecks++; $passedChecks++ }
        if ($dependencyValidation.VisualStudioAvailable) { $totalChecks++; $passedChecks++ }
        if ($dependencyValidation.PowerShellAvailable) { $totalChecks++; $passedChecks++ }
    }
    
    $validationResults.PassedChecks = $passedChecks
    $validationResults.TotalChecks = $totalChecks
    
    if ($totalChecks -gt 0) {
        $validationResults.ValidationScore = [math]::Round(($passedChecks / $totalChecks) * 100, 1)
    }
    
    # Determine overall status
    if ($validationResults.ValidationScore -ge 90) {
        $validationResults.Status = "Excellent"
    } elseif ($validationResults.ValidationScore -ge 75) {
        $validationResults.Status = "Good"
    } elseif ($validationResults.ValidationScore -ge 60) {
        $validationResults.Status = "Fair"
    } else {
        $validationResults.Status = "Poor"
    }
    
    # Store results
    $ValidationResults[$ComponentName] = $validationResults
    
    return $validationResults
}

# Generate QA validation report
function Generate-QAValidationReport {
    param([string]$ReportPath)
    
    Write-Log "Generating QA validation report..."
    
    $report = @"
# uevr Cross-Engine VR Adapters - QA Validation Report

## Report Generated: $(Get-Date -Format 'yyyy-MM-dd HH:mm:ss')
## Validation Mode: $ValidationMode
## Target Components: $TargetComponent

## Executive Summary

"@
    
    # Overall statistics
    $totalComponents = 0
    $totalIssues = 0
    $totalWarnings = 0
    $overallValidationScore = 0
    $componentsPassed = 0
    
    foreach ($component in $ValidationResults.Keys) {
        if ($component -ne "Overall" -and $ValidationResults[$component].Count -gt 0) {
            $results = $ValidationResults[$component]
            $totalComponents++
            $totalIssues += $results.Issues.Count
            $totalWarnings += $results.Warnings.Count
            $overallValidationScore += $results.ValidationScore
            
            if ($results.ValidationScore -ge 75) {
                $componentsPassed++
            }
        }
    }
    
    if ($totalComponents -gt 0) {
        $overallValidationScore = [math]::Round($overallValidationScore / $totalComponents, 1)
    }
    
    $report += @"

### Overall QA Status
- **Components Validated:** $totalComponents
- **Components Passed:** $componentsPassed
- **Components Failed:** $($totalComponents - $componentsPassed)
- **Overall Validation Score:** $overallValidationScore%
- **Total Issues Found:** $totalIssues
- **Total Warnings:** $totalWarnings
- **QA Status:** $(if ($overallValidationScore -ge 90) { "✅ EXCELLENT" } elseif ($overallValidationScore -ge 75) { "✅ PASSED" } elseif ($overallValidationScore -ge 60) { "⚠️ CONDITIONAL PASS" } else { "❌ FAILED" })

## Component Validation Results

"@
    
    # Individual component validation
    foreach ($component in $ValidationResults.Keys) {
        if ($component -ne "Overall" -and $ValidationResults[$component].Count -gt 0) {
            $results = $ValidationResults[$component]
            
            $report += @"

### $component
- **Status:** $($results.Status)
- **Validation Score:** $($results.ValidationScore)%
- **Passed Checks:** $($results.PassedChecks)/$($results.TotalChecks)
- **Issues:** $($results.Issues.Count)
- **Warnings:** $($results.Warnings.Count)

#### Build Validation
- **Build Success:** $($results.BuildValidation.BuildSuccess)
- **Success Rate:** $($results.BuildValidation.SuccessRate)%
- **Build Time:** $($results.BuildValidation.BuildTime)
- **Errors:** $($results.BuildValidation.ErrorCount)
- **Warnings:** $($results.BuildValidation.WarningCount)

#### Deployment Validation
- **Profile Directory:** $($results.DeploymentValidation.ProfileDirectoryExists)
- **Adapter DLL:** $($results.DeploymentValidation.AdapterDLLExists)
- **DLL Size:** $($results.DeploymentValidation.AdapterDLLSize) bytes
- **Deployment Time:** $($results.DeploymentValidation.DeploymentTime)

#### Dependency Validation
- **CMake:** $($results.DependencyValidation.CMakeAvailable)
- **Visual Studio:** $($results.DependencyValidation.VisualStudioAvailable)
- **PowerShell:** $($results.DependencyValidation.PowerShellAvailable)
- **Missing Dependencies:** $($results.DependencyValidation.MissingDependencies.Count)

#### Issues:
$($results.Issues | ForEach-Object { "- $_" } | Out-String)

#### Warnings:
$($results.Warnings | ForEach-Object { "- $_" } | Out-String)

"@
        }
    }
    
    # QA recommendations
    $report += @"

## QA Recommendations

### Immediate Actions
"@
    
    if ($totalIssues -gt 0) {
        $report += @"
- **Critical:** Address all validation issues before production deployment
- **Review:** Examine failed validation checks for systematic problems
- **Test:** Re-run validation after fixing issues
- **Document:** Update build procedures based on findings
"@
    } else {
        $report += @"
- **Status:** All components pass QA validation
- **Maintenance:** Continue monitoring for quality degradation
- **Optimization:** Consider performance improvements for high-scoring components
"@
    }
    
    $report += @"

### Quality Standards
- **Build Success Rate:** Maintain ≥90% success rate
- **Error Tolerance:** Zero critical errors allowed
- **Warning Limits:** Keep warnings under 5 per build
- **Deployment Verification:** Ensure all adapters deploy correctly
- **Dependency Management:** Verify all required tools available

### Continuous Improvement
- **Regular Validation:** Run QA validation after each build
- **Performance Tracking:** Monitor build times and success rates
- **Issue Tracking:** Document and resolve validation failures
- **Process Updates:** Refine build procedures based on findings

## Next Steps

1. **Address Issues:** Fix all validation failures
2. **Re-validate:** Run validation again after fixes
3. **Process Review:** Update build procedures if needed
4. **Quality Monitoring:** Establish regular validation schedule

---

**Report Generated by:** $ToolName  
**Version:** 1.0.0  
**Status:** QA Validation Complete
"@
    
    # Save report
    Add-Content -Path $ReportPath -Value $report
    Write-Log "QA validation report saved to: $ReportPath"
}

# Main validation execution
try {
    Write-Log "=== Starting $ToolName ==="
    Write-Log "Validation Mode: $ValidationMode"
    Write-Log "Target Component: $TargetComponent"
    Write-Log "Generate QA Report: $GenerateQAReport"
    Write-Log "Validate Deployment: $ValidateDeployment"
    Write-Log "Check Dependencies: $CheckDependencies"
    Write-Log "Performance Validation: $PerformanceValidation"
    Write-Log ""
    
    # Determine components to validate
    $componentsToValidate = @()
    
    switch ($TargetComponent.ToLower()) {
        "all" {
            $componentsToValidate = @("MainFramework", "MTFramework", "REEngine", "REDengine4")
        }
        "main" {
            $componentsToValidate = @("MainFramework")
        }
        "mt" {
            $componentsToValidate = @("MTFramework")
        }
        "re" {
            $componentsToValidate = @("REEngine")
        }
        "red" {
            $componentsToValidate = @("REDengine4")
        }
        default {
            $componentsToValidate = @("MainFramework", "MTFramework", "REEngine", "REDengine4")
        }
    }
    
    # Component paths
    $componentPaths = @{
        "MainFramework" = @{
            BuildTools = "$BuildToolsDir/build_main_cross_engine"
            Logs = "$LogsDir/main_cross_engine"
            Profile = "$ProfilesDir/main_cross_engine"
        }
        "MTFramework" = @{
            BuildTools = "$BuildToolsDir/build_mt_framework"
            Logs = "$LogsDir/mt_framework"
            Profile = "$ProfilesDir/MonsterHunterWorld"
        }
        "REEngine" = @{
            BuildTools = "$BuildToolsDir/build_re_engine"
            Logs = "$LogsDir/re_engine"
            Profile = "$ProfilesDir/re7"
        }
        "REDengine4" = @{
            BuildTools = "$BuildToolsDir/build_redengine4"
            Logs = "$LogsDir/redengine4"
            Profile = "$ProfilesDir/Cyberpunk2077"
        }
    }
    
    # Validate each component
    foreach ($component in $componentsToValidate) {
        $componentPath = $componentPaths[$component]
        
        Write-Log "=== Processing $component ==="
        
        # Validate component
        $results = Validate-Component -ComponentName $component -ComponentPath $componentPath.BuildTools -LogPath $componentPath.Logs -ProfilePath $componentPath.Profile
        
        Write-Log ""
    }
    
    # Generate QA report if requested
    if ($GenerateQAReport) {
        $reportPath = "$ReportsDir/qa_validation_report_$(Get-Date -Format 'yyyyMMdd_HHmmss').md"
        Generate-QAValidationReport -ReportPath $reportPath
    }
    
    # Validation summary
    Write-Log ""
    Write-Log "=== Validation Summary ==="
    
    $totalComponents = 0
    $totalIssues = 0
    $totalWarnings = 0
    $overallScore = 0
    $componentsPassed = 0
    
    foreach ($component in $componentsToValidate) {
        if ($ValidationResults.ContainsKey($component)) {
            $results = $ValidationResults[$component]
            $totalComponents++
            $totalIssues += $results.Issues.Count
            $totalWarnings += $results.Warnings.Count
            $overallScore += $results.ValidationScore
            
            if ($results.ValidationScore -ge 75) {
                $componentsPassed++
            }
            
            Write-Log "$component: $($results.Status) - $($results.ValidationScore)% validation score"
        }
    }
    
    if ($totalComponents -gt 0) {
        $overallScore = [math]::Round($overallScore / $totalComponents, 1)
    }
    
    Write-Log ""
    Write-Log "Overall Results:"
    Write-Log "- Components Validated: $totalComponents"
    Write-Log "- Components Passed: $componentsPassed"
    Write-Log "- Components Failed: $($totalComponents - $componentsPassed)"
    Write-Log "- Total Issues: $totalIssues"
    Write-Log "- Total Warnings: $totalWarnings"
    Write-Log "- Overall Validation Score: $overallScore%"
    
    if ($overallScore -ge 90) {
        Write-Log "🎉 Excellent QA validation results!"
    } elseif ($overallScore -ge 75) {
        Write-Log "✅ QA validation passed successfully"
    } elseif ($overallScore -ge 60) {
        Write-Log "⚠️  QA validation passed with conditions"
    } else {
        Write-Log "❌ QA validation failed - immediate action required"
    }
    
} catch {
    Write-Log "Validation failed: $($_.Exception.Message)" "ERROR"
    Write-Log "Error Details: $($_.Exception.ToString())" "ERROR"
    exit 1
}
