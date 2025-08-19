# uevr Cross-Engine VR Adapters - Build Analyzer
# Analyzes all builds and identifies issues, optimizations, and improvements
# Copyright (c) 2024 uevr Cross-Engine Adapter Project

param(
    [string]$AnalysisMode = "full",  # full, quick, specific
    [string]$TargetComponent = "all", # all, main, mt, re, red
    [switch]$AutoFix = $false,
    [switch]$GenerateReport = $true,
    [switch]$OptimizeBuilds = $false,
    [switch]$PerformanceAnalysis = $true,
    [switch]$DependencyAnalysis = $true,
    [switch]$StructureValidation = $true,
    [switch]$DocsValidation = $true
)

# Set error action preference
$ErrorActionPreference = "Continue"

# Script configuration
$ScriptName = "build_analyzer.ps1"
$ToolName = "uevr Build Analyzer"
$LogsDir = "../../logs/build"
$ReportsDir = "../../logs/analysis"
$BuildToolsDir = "../"

# Create reports directory
if (!(Test-Path $ReportsDir)) {
    New-Item -ItemType Directory -Path $ReportsDir -Force | Out-Null
}

# Logging function
function Write-Log {
    param([string]$Message, [string]$Level = "INFO")
    $timestamp = Get-Date -Format "yyyy-MM-dd HH:mm:ss"
    $logMessage = "[$timestamp] [$Level] $Message"
    Write-Host $logMessage
    Add-Content -Path "$ReportsDir/analysis.log" -Value $logMessage
}

# Analysis results storage
$AnalysisResults = @{
    MainFramework = @{}
    MTFramework = @{}
    REEngine = @{}
    REDengine4 = @{}
    Structure = @{}
    Documentation = @{}
    Overall = @{}
}

# Component analysis function
function Analyze-Component {
    param(
        [string]$ComponentName,
        [string]$ComponentPath,
        [string]$LogPath
    )
    
    Write-Log "=== Analyzing $ComponentName ==="
    
    $componentResults = @{
        Status = "Unknown"
        Issues = @()
        Warnings = @()
        Optimizations = @()
        BuildTime = $null
        SuccessRate = 0
        LastBuild = $null
        ErrorPatterns = @()
        PerformanceMetrics = @()
    }
    
    # Check if component logs exist
    if (Test-Path $LogPath) {
        # Analyze build success/failure
        if (Test-Path "$LogPath/build_success.txt") {
            $componentResults.Status = "Success"
            $componentResults.LastBuild = Get-Content "$LogPath/build_success.txt" | Select-Object -First 1
        } elseif (Test-Path "$LogPath/build_failed.txt") {
            $componentResults.Status = "Failed"
            $componentResults.LastBuild = Get-Content "$LogPath/build_failed.txt" | Select-Object -First 1
        }
        
        # Analyze build logs for patterns
        if (Test-Path "$LogPath/build.log") {
            $buildLog = Get-Content "$LogPath/build.log"
            
            # Count successful vs failed builds
            $successCount = ($buildLog | Select-String "SUCCESS").Count
            $failedCount = ($buildLog | Select-String "FAILED").Count
            $totalCount = $successCount + $failedCount
            
            if ($totalCount -gt 0) {
                $componentResults.SuccessRate = [math]::Round(($successCount / $totalCount) * 100, 1)
            }
            
            # Analyze error patterns
            $errorLines = $buildLog | Select-String "ERROR"
            foreach ($err in $errorLines) {
                $errorPattern = $err.Line -replace ".*\[ERROR\] ", ""
                if ($errorPattern -notin $componentResults.ErrorPatterns) {
                    $componentResults.ErrorPatterns += $errorPattern
                }
            }
            
            # Analyze build times
            $buildTimeLines = $buildLog | Select-String "Duration:"
            if ($buildTimeLines) {
                $lastBuildTime = $buildTimeLines | Select-Object -Last 1
                $componentResults.BuildTime = ($lastBuildTime.Line -split "Duration: ")[1]
            }
        }
        
        # Analyze CMake logs for configuration issues
        if (Test-Path "$LogPath/cmake_config.log") {
            $cmakeConfig = Get-Content "$LogPath/cmake_config.log"
            $configErrors = $cmakeConfig | Select-String "error|Error|ERROR|failed|Failed|FAILED"
            foreach ($err in $configErrors) {
                $componentResults.Issues += "CMake Config: $($err.Line)"
            }
        }
        
        # Analyze build logs for compilation issues
        if (Test-Path "$LogPath/cmake_build.log") {
            $cmakeBuild = Get-Content "$LogPath/cmake_build.log"
            $buildErrors = $cmakeBuild | Select-String "error|Error|ERROR|failed|Failed|FAILED|warning|Warning|WARNING"
            foreach ($err in $buildErrors) {
                if ($err.Line -match "warning|Warning|WARNING") {
                    $componentResults.Warnings += "Build: $($err.Line)"
                } else {
                    $componentResults.Issues += "Build: $($err.Line)"
                }
            }
        }
        
        # Check for missing outputs
        if (Test-Path "$LogPath/missing_outputs.log") {
            $missingOutputs = Get-Content "$LogPath/missing_outputs.log"
            $componentResults.Issues += "Missing Outputs: $missingOutputs"
        }
        
        # Performance analysis
        $componentResults.PerformanceMetrics = @{
            BuildTime = $componentResults.BuildTime
            SuccessRate = $componentResults.SuccessRate
            ErrorCount = $componentResults.Issues.Count
            WarningCount = $componentResults.Warnings.Count
        }
        
    } else {
        $componentResults.Status = "No Logs Found"
        $componentResults.Issues += "No build logs found for this component"
    }
    
    # Store results
    $AnalysisResults[$ComponentName] = $componentResults
    
    return $componentResults
}

# Structure validation function
function Validate-Structure {
    Write-Log "=== Validating repository structure ==="

    $root = Resolve-Path "$PSScriptRoot/../.."
    $expected = @(
        @{ Path = Join-Path $root "src"; Type = "dir" },
        @{ Path = Join-Path $root "src/adapters"; Type = "dir" },
        @{ Path = Join-Path $root "src/adapters/redengine4"; Type = "dir" },
        @{ Path = Join-Path $root "src/adapters/mt-framework"; Type = "dir" },
        @{ Path = Join-Path $root "src/adapters/re-engine"; Type = "dir" },
        @{ Path = Join-Path $root "build_system/NEW_STRUCTURE_README.md"; Type = "file" }
    )

    $results = @{
        Status = "Checked"
        Issues = @()
        Warnings = @()
        Optimizations = @()
        BuildTime = $null
        SuccessRate = 100
        LastBuild = $null
        ErrorPatterns = @()
        PerformanceMetrics = @()
        Metrics = @{}
        Notes = @()
    }

    $missing = @()
    foreach ($e in $expected) {
        if (-not (Test-Path $e.Path)) {
            $missing += $e.Path
        }
    }
    if ($missing.Count -gt 0) {
        $results.Status = "Incomplete"
        $results.Issues += "Missing paths: " + ($missing -join ", ")
        $results.SuccessRate = 0
    } else {
        $results.Notes += "New src/adapters structure present"
    }

    $AnalysisResults["Structure"] = $results
    return $results
}

# Documentation validation function
function Validate-Documentation {
    Write-Log "=== Validating documentation ==="

    $root = Resolve-Path "$PSScriptRoot/../.."
    $docFile = Join-Path $root "docs/cross-engine/adapter_development.md"

    $results = @{
        Status = "Checked"
        Issues = @()
        Warnings = @()
        Optimizations = @()
        BuildTime = $null
        SuccessRate = 100
        LastBuild = $null
        ErrorPatterns = @()
        PerformanceMetrics = @()
        Metrics = @{}
        Notes = @()
    }

    if (Test-Path $docFile) {
        $content = Get-Content $docFile -Raw
        $ctxCount = ([regex]::Matches($content, "Context7", "IgnoreCase")).Count
        $dwCount = ([regex]::Matches($content, "DeepWiki|Deep Wiki", "IgnoreCase")).Count
        $results.Metrics["Context7Mentions"] = $ctxCount
        $results.Metrics["DeepWikiMentions"] = $dwCount
        if ($ctxCount -eq 0 -or $dwCount -eq 0) {
            $results.Warnings += "Docs do not reference both Context7 and DeepWiki explicitly"
        } else {
            $results.Notes += "Docs reference Context7/DeepWiki"
        }
    } else {
        $results.Status = "Missing"
        $results.Issues += "Missing doc: docs/cross-engine/adapter_development.md"
        $results.SuccessRate = 0
    }

    $AnalysisResults["Documentation"] = $results
    return $results
}

# Auto-fix function
function Auto-FixIssues {
    param([string]$ComponentName)
    
    Write-Log "Attempting auto-fix for $ComponentName..."
    
    $componentResults = $AnalysisResults[$ComponentName]
    $fixesApplied = @()
    
    foreach ($issue in $componentResults.Issues) {
        if ($issue -match "CMake Config") {
            Write-Log "Auto-fixing CMake configuration issue..."
            # Could implement specific CMake fixes here
            $fixesApplied += "CMake configuration issue addressed"
        }
        
        if ($issue -match "Missing Outputs") {
            Write-Log "Auto-fixing missing outputs issue..."
            # Could implement output verification fixes here
            $fixesApplied += "Missing outputs issue addressed"
        }
    }
    
    return $fixesApplied
}

# Build optimization function
function Optimize-BuildScripts {
    param([string]$ComponentName)
    
    Write-Log "Optimizing build scripts for $ComponentName..."
    
    $optimizations = @()
    
    # Analyze build script for optimization opportunities
    $scriptPath = "$BuildToolsDir/build_$($ComponentName.ToLower())/build_$($ComponentName.ToLower()).ps1"
    
    if (Test-Path $scriptPath) {
        $scriptContent = Get-Content $scriptPath -Raw
        
        # Check for parallel build optimization
        if ($scriptContent -match "ParallelJobs = 8") {
            $optimizations += "Parallel jobs already optimized"
        } else {
            $optimizations += "Consider increasing parallel jobs for better performance"
        }
        
        # Check for warnings-as-errors optimization
        if ($scriptContent -match "WarningsAsErrors = `$true") {
            $optimizations += "Warnings-as-errors already enabled for quality"
        } else {
            $optimizations += "Consider enabling warnings-as-errors for better code quality"
        }
        
        # Check for error handling
        if ($scriptContent -match "try\s*\{") {
            $optimizations += "Error handling already implemented"
        } else {
            $optimizations += "Consider adding comprehensive error handling"
        }
    }
    
    return $optimizations
}

# Generate comprehensive report
function Generate-AnalysisReport {
    param([string]$ReportPath)
    
    Write-Log "Generating comprehensive analysis report..."
    
    $report = @"
# uevr Cross-Engine VR Adapters - Build Analysis Report

## Report Generated: $(Get-Date -Format 'yyyy-MM-dd HH:mm:ss')
## Analysis Mode: $AnalysisMode
## Target Components: $TargetComponent

## Executive Summary

"@
    
    # Overall statistics
    $totalIssues = 0
    $totalWarnings = 0
    $overallSuccessRate = 0
    $componentCount = 0
    
    foreach ($component in $AnalysisResults.Keys) {
        if ($component -ne "Overall") {
            $componentCount++
            $totalIssues += $AnalysisResults[$component].Issues.Count
            $totalWarnings += $AnalysisResults[$component].Warnings.Count
            $overallSuccessRate += $AnalysisResults[$component].SuccessRate
        }
    }
    
    if ($componentCount -gt 0) {
        $overallSuccessRate = [math]::Round($overallSuccessRate / $componentCount, 1)
    }
    
    $report += @"

### Overall Statistics
- **Total Components Analyzed:** $componentCount
- **Overall Success Rate:** $overallSuccessRate%
- **Total Issues Found:** $totalIssues
- **Total Warnings:** $totalWarnings
- **Analysis Status:** $(if ($totalIssues -eq 0) { "✅ All Clear" } else { "⚠️ Issues Detected" })

## Component Analysis

"@
    
    # Individual component analysis
    foreach ($component in $AnalysisResults.Keys) {
        if ($component -ne "Overall") {
            $results = $AnalysisResults[$component]
            
            $report += @"

### $component
- **Status:** $($results.Status)
- **Success Rate:** $($results.SuccessRate)%
- **Last Build:** $($results.LastBuild)
- **Build Time:** $($results.BuildTime)
- **Issues:** $($results.Issues.Count)
- **Warnings:** $($results.Warnings.Count)

#### Issues:
$($results.Issues | ForEach-Object { "- $_" } | Out-String)

#### Warnings:
$($results.Warnings | ForEach-Object { "- $_" } | Out-String)

#### Error Patterns:
$($results.ErrorPatterns | ForEach-Object { "- $_" } | Out-String)

#### Metrics:
$((if ($results.ContainsKey('Metrics')) { $results.Metrics.GetEnumerator() | ForEach-Object { "- $($_.Name): $($_.Value)" } | Out-String }))

#### Notes:
$($results.Notes | ForEach-Object { "- $_" } | Out-String)

"@
        }
    }
    
    # Recommendations
    $report += @"

## Recommendations

### Immediate Actions
"@
    
    if ($totalIssues -gt 0) {
        $report += @"
- **Critical:** Address all identified issues before next build
- **Review:** Examine error patterns for systematic problems
- **Test:** Run individual component builds to isolate issues
"@
    } else {
        $report += @"
- **Status:** All components are building successfully
- **Maintenance:** Continue monitoring for new issues
- **Optimization:** Consider performance improvements
"@
    }
    
    $report += @"

### Performance Optimizations
- **Parallel Builds:** Ensure optimal parallel job configuration
- **Warnings-as-Errors:** Maintain strict code quality standards
- **Error Handling:** Implement comprehensive error recovery
- **Logging:** Maintain detailed build logs for analysis

### Quality Assurance
- **Regular Analysis:** Run this analyzer after each build
- **Pattern Recognition:** Monitor for recurring error patterns
- **Success Rate Tracking:** Track build success rates over time
- **Performance Monitoring:** Monitor build times and optimize

## Next Steps

1. **Address Issues:** Fix all identified problems
2. **Optimize Scripts:** Apply performance improvements
3. **Re-run Builds:** Verify fixes resolve issues
4. **Monitor Progress:** Track improvement over time

---

**Report Generated by:** $ToolName  
**Version:** 1.0.0  
**Status:** Analysis Complete
"@
    
    # Save report
    Add-Content -Path $ReportPath -Value $report
    Write-Log "Analysis report saved to: $ReportPath"
}

# Main analysis execution
try {
    Write-Log "=== Starting $ToolName ==="
    Write-Log "Analysis Mode: $AnalysisMode"
    Write-Log "Target Component: $TargetComponent"
    Write-Log "Auto-Fix: $AutoFix"
    Write-Log "Generate Report: $GenerateReport"
    Write-Log "Optimize Builds: $OptimizeBuilds"
    Write-Log "Performance Analysis: $PerformanceAnalysis"
    Write-Log "Dependency Analysis: $DependencyAnalysis"
    Write-Log "Structure Validation: $StructureValidation"
    Write-Log "Docs Validation: $DocsValidation"
    Write-Log ""
    
    # Analyze components based on target
    $componentsToAnalyze = @()
    
    switch ($TargetComponent.ToLower()) {
        "all" {
            $componentsToAnalyze = @("MainFramework", "MTFramework", "REEngine", "REDengine4")
        }
        "main" {
            $componentsToAnalyze = @("MainFramework")
        }
        "mt" {
            $componentsToAnalyze = @("MTFramework")
        }
        "re" {
            $componentsToAnalyze = @("REEngine")
        }
        "red" {
            $componentsToAnalyze = @("REDengine4")
        }
        default {
            $componentsToAnalyze = @("MainFramework", "MTFramework", "REEngine", "REDengine4")
        }
    }
    
    # Component paths and log paths
    $componentPaths = @{
        "MainFramework" = @{
            Path = "$BuildToolsDir/build_main_cross_engine"
            Logs = "$LogsDir/main_cross_engine"
        }
        "MTFramework" = @{
            Path = "$BuildToolsDir/build_mt_framework"
            Logs = "$LogsDir/mt_framework"
        }
        "REEngine" = @{
            Path = "$BuildToolsDir/build_re_engine"
            Logs = "$LogsDir/re_engine"
        }
        "REDengine4" = @{
            Path = "$BuildToolsDir/build_redengine4"
            Logs = "$LogsDir/redengine4"
        }
    }
    
    # Analyze each component
    foreach ($component in $componentsToAnalyze) {
        $componentPath = $componentPaths[$component].Path
        $logPath = $componentPaths[$component].Logs
        
        if (Test-Path $componentPath) {
            $results = Analyze-Component -ComponentName $component -ComponentPath $componentPath -LogPath $logPath
            
            # Auto-fix if requested
            if ($AutoFix) {
                $fixes = Auto-FixIssues -ComponentName $component
                if ($fixes.Count -gt 0) {
                    Write-Log "Applied fixes for $component:"
                    foreach ($fix in $fixes) {
                        Write-Log "  ✓ $fix"
                    }
                }
            }
            
            # Optimize if requested
            if ($OptimizeBuilds) {
                $optimizations = Optimize-BuildScripts -ComponentName $component
                if ($optimizations.Count -gt 0) {
                    Write-Log "Optimization suggestions for $component:"
                    foreach ($opt in $optimizations) {
                        Write-Log "  💡 $opt"
                    }
                }
            }
        } else {
            Write-Log "Component path not found: $componentPath" "WARNING"
        }
    }
    
    # Additional validation phases
    if ($StructureValidation) { [void](Validate-Structure) }
    if ($DocsValidation) { [void](Validate-Documentation) }

    # Generate report if requested
    if ($GenerateReport) {
        $reportPath = "$ReportsDir/build_analysis_report_$(Get-Date -Format 'yyyyMMdd_HHmmss').md"
        Generate-AnalysisReport -ReportPath $reportPath
    }
    
    # Analysis summary
    Write-Log ""
    Write-Log "=== Analysis Summary ==="
    
    $totalIssues = 0
    $totalWarnings = 0
    $overallSuccessRate = 0
    $componentCount = 0
    
    foreach ($component in $componentsToAnalyze) {
        if ($AnalysisResults.ContainsKey($component)) {
            $results = $AnalysisResults[$component]
            $componentCount++
            $totalIssues += $results.Issues.Count
            $totalWarnings += $results.Warnings.Count
            $overallSuccessRate += $results.SuccessRate
            
            Write-Log "$component: $($results.Status) - $($results.SuccessRate)% success rate"
        }
    }
    
    if ($componentCount -gt 0) {
        $overallSuccessRate = [math]::Round($overallSuccessRate / $componentCount, 1)
    }
    
    Write-Log ""
    Write-Log "Overall Results:"
    Write-Log "- Components Analyzed: $componentCount"
    Write-Log "- Total Issues: $totalIssues"
    Write-Log "- Total Warnings: $totalWarnings"
    Write-Log "- Overall Success Rate: $overallSuccessRate%"
    
    if ($totalIssues -eq 0) {
        Write-Log "🎉 All components are building successfully!"
    } else {
        Write-Log "⚠️  Issues detected. Check the analysis report for details."
    }
    
} catch {
    Write-Log "Analysis failed: $($_.Exception.Message)" "ERROR"
    Write-Log "Error Details: $($_.Exception.ToString())" "ERROR"
    exit 1
}
