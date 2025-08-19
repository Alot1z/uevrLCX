# uevr Cross-Engine VR Adapters - Build Optimizer
# Optimizes build scripts for better performance and quality
# Copyright (c) 2024 uevr Cross-Engine Adapter Project

param(
    [string]$TargetComponent = "all", # all, main, mt, re, red
    [switch]$ApplyOptimizations = $false,
    [switch]$BackupOriginal = $true,
    [switch]$GenerateReport = $true,
    [switch]$PerformanceMode = $false
)

# Set error action preference
$ErrorActionPreference = "Continue"

# Script configuration
$ScriptName = "build_optimizer.ps1"
$ToolName = "uevr Build Optimizer"
$BuildToolsDir = "../"
$ReportsDir = "../../logs/optimization"
$BackupDir = "../../logs/optimization/backups"

# Create directories
if (!(Test-Path $ReportsDir)) {
    New-Item -ItemType Directory -Path $ReportsDir -Force | Out-Null
}
if (!(Test-Path $BackupDir)) {
    New-Item -ItemType Directory -Path $BackupDir -Force | Out-Null
}

# Logging function
function Write-Log {
    param([string]$Message, [string]$Level = "INFO")
    $timestamp = Get-Date -Format "yyyy-MM-dd HH:mm:ss"
    $logMessage = "[$timestamp] [$Level] $Message"
    Write-Host $logMessage
    Add-Content -Path "$ReportsDir/optimization.log" -Value $logMessage
}

# Optimization results storage
$OptimizationResults = @{
    MainFramework = @{}
    MTFramework = @{}
    REEngine = @{}
    REDengine4 = @{}
    Overall = @{}
}

# Backup original file function
function Backup-OriginalFile {
    param([string]$FilePath)
    
    if ($BackupOriginal -and (Test-Path $FilePath)) {
        $fileName = Split-Path $FilePath -Leaf
        $backupPath = "$BackupDir/$fileName.backup.$(Get-Date -Format 'yyyyMMdd_HHmmss')"
        Copy-Item -Path $FilePath -Destination $backupPath -Force
        Write-Log "Backed up original file to: $backupPath"
        return $backupPath
    }
    return $null
}

# Analyze build script for optimization opportunities
function Analyze-BuildScript {
    param(
        [string]$ComponentName,
        [string]$ScriptPath
    )
    
    Write-Log "Analyzing build script for $ComponentName..."
    
    $analysis = @{
        ComponentName = $ComponentName
        ScriptPath = $ScriptPath
        Optimizations = @()
        Issues = @()
        Warnings = @()
        PerformanceScore = 0
        QualityScore = 0
        OverallScore = 0
    }
    
    if (!(Test-Path $ScriptPath)) {
        $analysis.Issues += "Script file not found: $ScriptPath"
        return $analysis
    }
    
    $scriptContent = Get-Content $ScriptPath -Raw
    $scriptLines = Get-Content $ScriptPath
    
    # Performance optimizations
    $performanceScore = 0
    $maxPerformanceScore = 100
    
    # Check parallel jobs configuration
    if ($scriptContent -match "ParallelJobs = 8") {
        $analysis.Optimizations += "✓ Parallel jobs already optimized (8 jobs)"
        $performanceScore += 25
    } elseif ($scriptContent -match "ParallelJobs = \d+") {
        $match = [regex]::Match($scriptContent, "ParallelJobs = (\d+)")
        $currentJobs = [int]$match.Groups[1].Value
        if ($currentJobs -lt 8) {
            $analysis.Optimizations += "💡 Increase parallel jobs from $currentJobs to 8 for better performance"
            $analysis.Warnings += "Suboptimal parallel job configuration"
        } else {
            $analysis.Optimizations += "✓ Parallel jobs well configured ($currentJobs jobs)"
            $performanceScore += 25
        }
    } else {
        $analysis.Optimizations += "⚠️ No parallel jobs configuration found"
        $analysis.Issues += "Missing parallel jobs configuration"
    }
    
    # Check build type optimization
    if ($scriptContent -match "BuildType.*Release") {
        $analysis.Optimizations += "✓ Release build type configured for optimal performance"
        $performanceScore += 20
    } else {
        $analysis.Optimizations += "💡 Consider using Release build type for production builds"
        $analysis.Warnings += "Build type not optimized for performance"
    }
    
    # Check CMake optimization flags
    if ($scriptContent -match "CMAKE_CXX_FLAGS.*/O2") {
        $analysis.Optimizations += "✓ CMake optimization flags enabled (/O2)"
        $performanceScore += 20
    } else {
        $analysis.Optimizations += "💡 Add CMake optimization flags for better performance"
        $analysis.Warnings += "Missing CMake optimization flags"
    }
    
    # Check for incremental build support
    if ($scriptContent -match "incremental|Incremental") {
        $analysis.Optimizations += "✓ Incremental build support detected"
        $performanceScore += 15
    } else {
        $analysis.Optimizations += "💡 Consider adding incremental build support"
        $analysis.Warnings += "No incremental build support"
    }
    
    # Quality optimizations
    $qualityScore = 0
    $maxQualityScore = 100
    
    # Check warnings-as-errors
    if ($scriptContent -match "WarningsAsErrors.*true") {
        $analysis.Optimizations += "✓ Warnings-as-errors enabled for code quality"
        $qualityScore += 25
    } else {
        $analysis.Optimizations += "💡 Enable warnings-as-errors for better code quality"
        $analysis.Warnings += "Warnings not treated as errors"
    }
    
    # Check error handling
    if ($scriptContent -match "try\s*\{") {
        $analysis.Optimizations += "✓ Comprehensive error handling implemented"
        $qualityScore += 25
    } else {
        $analysis.Optimizations += "💡 Add comprehensive error handling"
        $analysis.Issues += "Missing error handling"
    }
    
    # Check logging
    if ($scriptContent -match "Write-Log|logging|Logging") {
        $analysis.Optimizations += "✓ Logging system implemented"
        $qualityScore += 20
    } else {
        $analysis.Optimizations += "💡 Add logging system for better debugging"
        $analysis.Warnings += "No logging system"
    }
    
    # Check validation
    if ($scriptContent -match "verify|Verify|validation|Validation") {
        $analysis.Optimizations += "✓ Build validation implemented"
        $qualityScore += 20
    } else {
        $analysis.Optimizations += "💡 Add build output validation"
        $analysis.Warnings += "No build validation"
    }
    
    # Check deployment
    if ($scriptContent -match "deploy|Deploy|copy|Copy") {
        $analysis.Optimizations += "✓ Automatic deployment implemented"
        $qualityScore += 10
    } else {
        $analysis.Optimizations += "💡 Add automatic deployment to profile directories"
        $analysis.Warnings += "No automatic deployment"
    }
    
    # Calculate scores
    $analysis.PerformanceScore = [math]::Round(($performanceScore / $maxPerformanceScore) * 100, 1)
    $analysis.QualityScore = [math]::Round(($qualityScore / $maxQualityScore) * 100, 1)
    $analysis.OverallScore = [math]::Round((($performanceScore + $qualityScore) / ($maxPerformanceScore + $maxQualityScore)) * 100, 1)
    
    return $analysis
}

# Apply optimizations to build script
function Apply-BuildScriptOptimizations {
    param(
        [string]$ComponentName,
        [string]$ScriptPath,
        [hashtable]$Analysis
    )
    
    Write-Log "Applying optimizations to $ComponentName build script..."
    
    if (!(Test-Path $ScriptPath)) {
        Write-Log "Script file not found: $ScriptPath" "ERROR"
        return $false
    }
    
    # Backup original file
    $backupPath = Backup-OriginalFile -FilePath $ScriptPath
    
    try {
        $scriptContent = Get-Content $ScriptPath -Raw
        $modified = $false
        
        # Apply performance optimizations
        if ($PerformanceMode) {
            # Optimize parallel jobs if not already optimal
            if ($scriptContent -notmatch "ParallelJobs = 8") {
                $scriptContent = $scriptContent -replace "ParallelJobs = \d+", "ParallelJobs = 8"
                $scriptContent = $scriptContent -replace "ParallelJobs", "ParallelJobs = 8"
                $modified = $true
                Write-Log "Applied parallel jobs optimization"
            }
            
            # Add CMake optimization flags if missing
            if ($scriptContent -notmatch "CMAKE_CXX_FLAGS.*/O2") {
                $scriptContent = $scriptContent -replace 'CMAKE_CXX_FLAGS="([^"]*)"', 'CMAKE_CXX_FLAGS="$1 /O2"'
                $modified = $true
                Write-Log "Applied CMake optimization flags"
            }
        }
        
        # Apply quality optimizations
        # Enable warnings-as-errors if not already enabled
        if ($scriptContent -notmatch "WarningsAsErrors.*true") {
            $scriptContent = $scriptContent -replace 'WarningsAsErrors = \$false', 'WarningsAsErrors = $true'
            $modified = $true
            Write-Log "Applied warnings-as-errors optimization"
        }
        
        # Add error handling if missing
        if ($scriptContent -notmatch "try\s*\{") {
            # This would require more complex script modification
            Write-Log "Error handling optimization requires manual implementation" "WARNING"
        }
        
        # Add logging if missing
        if ($scriptContent -notmatch "Write-Log|logging|Logging") {
            # This would require more complex script modification
            Write-Log "Logging optimization requires manual implementation" "WARNING"
        }
        
        # Save modified script if changes were made
        if ($modified) {
            Set-Content -Path $ScriptPath -Value $scriptContent -Force
            Write-Log "Optimizations applied successfully"
            return $true
        } else {
            Write-Log "No optimizations needed - script already optimized"
            return $true
        }
        
    } catch {
        Write-Log "Failed to apply optimizations: $($_.Exception.Message)" "ERROR"
        
        # Restore from backup if available
        if ($backupPath -and (Test-Path $backupPath)) {
            Copy-Item -Path $backupPath -Destination $ScriptPath -Force
            Write-Log "Restored original script from backup"
        }
        
        return $false
    }
}

# Generate optimization report
function Generate-OptimizationReport {
    param([string]$ReportPath)
    
    Write-Log "Generating optimization report..."
    
    $report = @"
# uevr Cross-Engine VR Adapters - Build Optimization Report

## Report Generated: $(Get-Date -Format 'yyyy-MM-dd HH:mm:ss')
## Target Components: $TargetComponent
## Performance Mode: $PerformanceMode
## Optimizations Applied: $ApplyOptimizations

## Executive Summary

"@
    
    # Overall statistics
    $totalOptimizations = 0
    $totalIssues = 0
    $totalWarnings = 0
    $overallPerformanceScore = 0
    $overallQualityScore = 0
    $overallScore = 0
    $componentCount = 0
    
    foreach ($component in $OptimizationResults.Keys) {
        if ($component -ne "Overall" -and $OptimizationResults[$component].Count -gt 0) {
            $results = $OptimizationResults[$component]
            $componentCount++
            $totalOptimizations += $results.Optimizations.Count
            $totalIssues += $results.Issues.Count
            $totalWarnings += $results.Warnings.Count
            $overallPerformanceScore += $results.PerformanceScore
            $overallQualityScore += $results.QualityScore
            $overallScore += $results.OverallScore
        }
    }
    
    if ($componentCount -gt 0) {
        $overallPerformanceScore = [math]::Round($overallPerformanceScore / $componentCount, 1)
        $overallQualityScore = [math]::Round($overallQualityScore / $componentCount, 1)
        $overallScore = [math]::Round($overallScore / $componentCount, 1)
    }
    
    $report += @"

### Overall Statistics
- **Components Analyzed:** $componentCount
- **Total Optimizations:** $totalOptimizations
- **Total Issues:** $totalIssues
- **Total Warnings:** $totalWarnings
- **Average Performance Score:** $overallPerformanceScore%
- **Average Quality Score:** $overallQualityScore%
- **Average Overall Score:** $overallScore%

## Component Analysis

"@
    
    # Individual component analysis
    foreach ($component in $OptimizationResults.Keys) {
        if ($component -ne "Overall" -and $OptimizationResults[$component].Count -gt 0) {
            $results = $OptimizationResults[$component]
            
            $report += @"

### $component
- **Performance Score:** $($results.PerformanceScore)%
- **Quality Score:** $($results.QualityScore)%
- **Overall Score:** $($results.OverallScore)%
- **Script Path:** $($results.ScriptPath)

#### Optimizations Applied:
$($results.Optimizations | ForEach-Object { "- $_" } | Out-String)

#### Issues Found:
$($results.Issues | ForEach-Object { "- $_" } | Out-String)

#### Warnings:
$($results.Warnings | ForEach-Object { "- $_" } | Out-String)

"@
        }
    }
    
    # Recommendations
    $report += @"

## Recommendations

### Performance Improvements
- **Parallel Builds:** Ensure optimal parallel job configuration (8+ jobs)
- **Build Type:** Use Release builds for production
- **CMake Flags:** Enable optimization flags (/O2, /MP)
- **Incremental Builds:** Implement incremental build support

### Quality Improvements
- **Warnings-as-Errors:** Enable strict code quality standards
- **Error Handling:** Implement comprehensive error recovery
- **Logging:** Add detailed logging for debugging
- **Validation:** Verify build outputs and deployment

### Maintenance
- **Regular Analysis:** Run optimizer after script changes
- **Performance Monitoring:** Track build times and optimize
- **Quality Tracking:** Monitor success rates and error patterns
- **Backup Strategy:** Always backup before applying optimizations

## Next Steps

1. **Review Optimizations:** Check applied changes
2. **Test Builds:** Verify optimizations work correctly
3. **Monitor Performance:** Track improvement in build times
4. **Quality Assurance:** Ensure code quality standards maintained

---

**Report Generated by:** $ToolName  
**Version:** 1.0.0  
**Status:** Optimization Complete
"@
    
    # Save report
    Add-Content -Path $ReportPath -Value $report
    Write-Log "Optimization report saved to: $ReportPath"
}

# Main optimization execution
try {
    Write-Log "=== Starting $ToolName ==="
    Write-Log "Target Component: $TargetComponent"
    Write-Log "Apply Optimizations: $ApplyOptimizations"
    Write-Log "Backup Original: $BackupOriginal"
    Write-Log "Generate Report: $GenerateReport"
    Write-Log "Performance Mode: $PerformanceMode"
    Write-Log ""
    
    # Determine components to optimize
    $componentsToOptimize = @()
    
    switch ($TargetComponent.ToLower()) {
        "all" {
            $componentsToOptimize = @("MainFramework", "MTFramework", "REEngine", "REDengine4")
        }
        "main" {
            $componentsToOptimize = @("MainFramework")
        }
        "mt" {
            $componentsToOptimize = @("MTFramework")
        }
        "re" {
            $componentsToOptimize = @("REEngine")
        }
        "red" {
            $componentsToOptimize = @("REDengine4")
        }
        default {
            $componentsToOptimize = @("MainFramework", "MTFramework", "REEngine", "REDengine4")
        }
    }
    
    # Component paths
    $componentPaths = @{
        "MainFramework" = "$BuildToolsDir/build_main_cross_engine/build_main_framework.ps1"
        "MTFramework" = "$BuildToolsDir/build_mt_framework/build_mt_framework.ps1"
        "REEngine" = "$BuildToolsDir/build_re_engine/build_re_engine.ps1"
        "REDengine4" = "$BuildToolsDir/build_redengine4/build_redengine4.ps1"
    }
    
    # Analyze and optimize each component
    foreach ($component in $componentsToOptimize) {
        $scriptPath = $componentPaths[$component]
        
        Write-Log "=== Processing $component ==="
        
        # Analyze build script
        $analysis = Analyze-BuildScript -ComponentName $component -ScriptPath $scriptPath
        $OptimizationResults[$component] = $analysis
        
        # Apply optimizations if requested
        if ($ApplyOptimizations) {
            $success = Apply-BuildScriptOptimizations -ComponentName $component -ScriptPath $scriptPath -Analysis $analysis
            if ($success) {
                Write-Log "✓ Optimizations applied successfully to $component"
            } else {
                Write-Log "✗ Failed to apply optimizations to $component" "ERROR"
            }
        }
        
        Write-Log ""
    }
    
    # Generate report if requested
    if ($GenerateReport) {
        $reportPath = "$ReportsDir/build_optimization_report_$(Get-Date -Format 'yyyyMMdd_HHmmss').md"
        Generate-OptimizationReport -ReportPath $reportPath
    }
    
    # Optimization summary
    Write-Log ""
    Write-Log "=== Optimization Summary ==="
    
    $totalOptimizations = 0
    $totalIssues = 0
    $totalWarnings = 0
    $overallScore = 0
    $componentCount = 0
    
    foreach ($component in $componentsToOptimize) {
        if ($OptimizationResults.ContainsKey($component)) {
            $results = $OptimizationResults[$component]
            $componentCount++
            $totalOptimizations += $results.Optimizations.Count
            $totalIssues += $results.Issues.Count
            $totalWarnings += $results.Warnings.Count
            $overallScore += $results.OverallScore
            
            Write-Log "$component: $($results.OverallScore)% overall score"
        }
    }
    
    if ($componentCount -gt 0) {
        $overallScore = [math]::Round($overallScore / $componentCount, 1)
    }
    
    Write-Log ""
    Write-Log "Overall Results:"
    Write-Log "- Components Processed: $componentCount"
    Write-Log "- Total Optimizations: $totalOptimizations"
    Write-Log "- Total Issues: $totalIssues"
    Write-Log "- Total Warnings: $totalWarnings"
    Write-Log "- Average Overall Score: $overallScore%"
    
    if ($overallScore -ge 90) {
        Write-Log "🎉 Excellent build script quality!"
    } elseif ($overallScore -ge 75) {
        Write-Log "👍 Good build script quality with room for improvement"
    } elseif ($overallScore -ge 60) {
        Write-Log "⚠️  Build scripts need optimization"
    } else {
        Write-Log "🚨 Build scripts require significant optimization"
    }
    
} catch {
    Write-Log "Optimization failed: $($_.Exception.Message)" "ERROR"
    Write-Log "Error Details: $($_.Exception.ToString())" "ERROR"
    exit 1
}
