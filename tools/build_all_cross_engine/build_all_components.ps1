# ss-Engine VR Adapters - Master Build Script
# Builds all 4 components: Main Framework + 3 Engine Adapters
# Copyright (c) 2024 ss-Engine Adapter Project

param(
    [string]$BuildType = "Release",
    [switch]$WarningsAsErrors = $true,
    [int]$ParallelJobs = 8,
    [switch]$SkipMainFramework = $false,
    [switch]$SkipMTFramework = $false,
    [switch]$SkipREEngine = $false,
    [switch]$SkipREDengine4 = $false
)

# Set error action preference
$ErrorActionPreference = "Stop"

# Script configuration
$ScriptName = "build_all_components.ps1"
$ProjectName = "ss-Engine VR Adapters - Complete System"
$LogsDir = "../../logs/build/all_components"
$StartTime = Get-Date

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

# Build component function
function Build-Component {
    param(
        [string]$ComponentName,
        [string]$ScriptPath,
        [string]$SkipFlag
    )
    
    if ($SkipFlag) {
        Write-Log "Skipping $ComponentName build (requested by user)"
        return $true
    }
    
    Write-Log "=== Building $ComponentName ==="
    
    try {
        # Run the component build script
        $buildArgs = @(
            "-ExecutionPolicy", "Bypass",
            "-File", $ScriptPath,
            "-BuildType", $BuildType,
            "-WarningsAsErrors:`$$WarningsAsErrors",
            "-ParallelJobs", $ParallelJobs
        )
        
        $buildOutput = & pwsh @buildArgs 2>&1
        if ($LASTEXITCODE -ne 0) {
            throw "Build failed with exit code $LASTEXITCODE`nOutput: $buildOutput"
        }
        
        Write-Log "✓ $ComponentName built successfully"
        return $true
        
    } catch {
        Write-ErrorLog "$ComponentName build failed: $($_.Exception.Message)" $_.Exception.ToString()
        return $false
    }
}

try {
    Write-Log "=== Starting $ProjectName Build ==="
    Write-Log "Build Type: $BuildType"
    Write-Log "Warnings as Errors: $WarningsAsErrors"
    Write-Log "Parallel Jobs: $ParallelJobs"
    Write-Log "Logs Directory: $LogsDir"
    Write-Log "Start Time: $StartTime"
    Write-Log ""
    
    # Build configuration summary
    Write-Log "Build Configuration:"
    Write-Log "- Main Cross-Engine Framework: $(-not $SkipMainFramework)"
    Write-Log "- MT Framework Adapter (Monster Hunter World): $(-not $SkipMTFramework)"
    Write-Log "- RE Engine Adapter (Resident Evil 7): $(-not $SkipREEngine)"
    Write-Log "- REDengine 4 Adapter (Cyberpunk 2077): $(-not $SkipREDengine4)"
    Write-Log ""
    
    # Component build results
    $buildResults = @{}
    
    # 1. Build Main Cross-Engine Framework
    $buildResults["Main Framework"] = Build-Component -ComponentName "Main Cross-Engine Framework" -ScriptPath "../build_main_cross_engine/build_main_framework.ps1" -SkipFlag $SkipMainFramework
    
    # 2. Build MT Framework Adapter
    $buildResults["MT Framework"] = Build-Component -ComponentName "MT Framework Adapter" -ScriptPath "../build_mt_framework/build_mt_framework.ps1" -SkipFlag $SkipMTFramework
    
    # 3. Build RE Engine Adapter
    $buildResults["RE Engine"] = Build-Component -ComponentName "RE Engine Adapter" -ScriptPath "../build_re_engine/build_re_engine.ps1" -SkipFlag $SkipREEngine
    
    # 4. Build REDengine 4 Adapter
    $buildResults["REDengine 4"] = Build-Component -ComponentName "REDengine 4 Adapter" -ScriptPath "../build_redengine4/build_redengine4.ps1" -SkipFlag $SkipREDengine4
    
    # Build summary
    $EndTime = Get-Date
    $BuildDuration = $EndTime - $StartTime
    
    Write-Log ""
    Write-Log "=== Build Summary ==="
    Write-Log "Project: $ProjectName"
    Write-Log "Start Time: $StartTime"
    Write-Log "End Time: $EndTime"
    Write-Log "Duration: $($BuildDuration.ToString('hh\:mm\:ss'))"
    Write-Log ""
    
    # Component results
    $successCount = 0
    $totalCount = 0
    
    foreach ($component in $buildResults.Keys) {
        $totalCount++
        if ($buildResults[$component]) {
            $successCount++
            Write-Log "✓ $component: SUCCESS"
        } else {
            Write-Log "✗ $component: FAILED"
        }
    }
    
    Write-Log ""
    Write-Log "Overall Results:"
    Write-Log "- Total Components: $totalCount"
    Write-Log "- Successful Builds: $successCount"
    Write-Log "- Failed Builds: $($totalCount - $successCount)"
    Write-Log "- Success Rate: $([math]::Round(($successCount / $totalCount) * 100, 1))%"
    
    # Final status
    if ($successCount -eq $totalCount) {
        Write-Log ""
        Write-Log "🎉 ALL COMPONENTS BUILT SUCCESSFULLY! 🎉"
        Write-Log "The complete ss-Engine VR Adapter system is ready!"
        Write-Log ""
        Write-Log "Deployed Components:"
        Write-Log "- Main Framework: include/build/"
        Write-Log "- MT Framework: profiles/MonsterHunterWorld/"
        Write-Log "- RE Engine: profiles/re7/"
        Write-Log "- REDengine 4: profiles/Cyberpunk2077/"
        
        # Create build success marker
        Add-Content -Path "$LogsDir/build_success.txt" -Value "All components built successfully at $(Get-Date -Format 'yyyy-MM-dd HH:mm:ss')"
        
    } else {
        Write-Log ""
        Write-Log "⚠️  SOME COMPONENTS FAILED TO BUILD ⚠️"
        Write-Log "Check the logs for failed components and fix the issues."
        Write-Log "Successful components are ready for use."
        
        # Create build partial success marker
        Add-Content -Path "$LogsDir/build_partial_success.txt" -Value "Partial build completed at $(Get-Date -Format 'yyyy-MM-dd HH:mm:ss'): $successCount/$totalCount components successful"
    }
    
    Write-Log ""
    Write-Log "Logs saved to: $LogsDir"
    Write-Log "Individual component logs available in their respective directories"
    
} catch {
    Write-ErrorLog "Master build script failed: $($_.Exception.Message)" $_.Exception.ToString()
    Write-Log "=== Build Summary ===" "ERROR"
    Write-Log "Project: $ProjectName" "ERROR"
    Write-Log "Status: CRITICAL FAILURE" "ERROR"
    Write-Log "Error: $($_.Exception.Message)" "ERROR"
    
    # Create build failure marker
    Add-Content -Path "$LogsDir/build_failed.txt" -Value "Master build failed at $(Get-Date -Format 'yyyy-MM-dd HH:mm:ss'): $($_.Exception.Message)"
    
    exit 1
}
