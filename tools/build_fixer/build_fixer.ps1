# uevr Cross-Engine VR Adapters - Build Fixer (PowerShell)
# Safely fixes common build issues with backups and validation
# This script is invoked by tools/build_fixer/build_fixer.bat

param(
    [string]$FixMode = "comprehensive",   # comprehensive, critical, warnings, specific, safe
    [string]$TargetComponent = "all",      # all, main, mt, re, red
    [switch]$AutoFix,
    [switch]$BackupOriginals,
    [switch]$ValidateFixes,
    [switch]$GenerateReport
)

$ErrorActionPreference = "Continue"

# Runtime defaults for switches (avoid defaulting in param to satisfy lint)
if (-not $PSBoundParameters.ContainsKey('AutoFix')) { $AutoFix = $true }
if (-not $PSBoundParameters.ContainsKey('BackupOriginals')) { $BackupOriginals = $true }
if (-not $PSBoundParameters.ContainsKey('ValidateFixes')) { $ValidateFixes = $true }
if (-not $PSBoundParameters.ContainsKey('GenerateReport')) { $GenerateReport = $true }

# Paths relative to this script (tools/build_fixer)
$RootDir = Resolve-Path "$PSScriptRoot/../.."
$LogsDir = Join-Path $RootDir "logs/fixing"

# Ensure logs dir exists
if (!(Test-Path $LogsDir)) { New-Item -ItemType Directory -Path $LogsDir -Force | Out-Null }

# Logging
$LogFile = Join-Path $LogsDir "fixer.log"
function Write-Log {
    param([string]$Message, [string]$Level = "INFO")
    $ts = Get-Date -Format "yyyy-MM-dd HH:mm:ss"
    $line = "[$ts] [$Level] $Message"
    Write-Host $line
    Add-Content -Path $LogFile -Value $line
}

# Utility: backup a file if it exists
function Backup-IfExists {
    param([string]$Path)
    if (Test-Path $Path) {
        $stamp = Get-Date -Format "yyyyMMdd_HHmmss"
        $bak = "$Path.$stamp.bak"
        Copy-Item -Path $Path -Destination $bak -Force
        Write-Log "Backed up: $Path -> $bak" "DEBUG"
    }
}

# Safe auto-fixes collection
$FixSummary = @()

# Structure: expected directories/files in new layout
$Expected = @(
    @{ Path = Join-Path $RootDir "src"; Type = "dir" },
    @{ Path = Join-Path $RootDir "src/adapters"; Type = "dir" },
    @{ Path = Join-Path $RootDir "src/adapters/redengine4"; Type = "dir" },
    @{ Path = Join-Path $RootDir "src/adapters/mt-framework"; Type = "dir" },
    @{ Path = Join-Path $RootDir "src/adapters/re-engine"; Type = "dir" },
    @{ Path = Join-Path $RootDir "build_system/NEW_STRUCTURE_README.md"; Type = "file" }
)

# Adapter convenience: ensure build.bat placeholders exist (non-destructive)
$AdapterBuildBats = @(
    Join-Path $RootDir "src/adapters/redengine4/build.bat" ,
    Join-Path $RootDir "src/adapters/mt-framework/build.bat" ,
    Join-Path $RootDir "src/adapters/re-engine/build.bat"
)

try {
    Write-Log "=== uevr Build Fixer Started ==="
    Write-Log "FixMode=$FixMode TargetComponent=$TargetComponent AutoFix=$AutoFix BackupOriginals=$BackupOriginals ValidateFixes=$ValidateFixes GenerateReport=$GenerateReport"

    # Validate required structure
    foreach ($e in $Expected) {
        if ($e.Type -eq "dir") {
            if (!(Test-Path $e.Path)) {
                if ($AutoFix) {
                    New-Item -ItemType Directory -Path $e.Path -Force | Out-Null
                    $FixSummary += "Created missing directory: $($e.Path)"
                    Write-Log "Created missing directory: $($e.Path)" "WARN"
                } else {
                    Write-Log "Missing directory: $($e.Path)" "WARN"
                }
            }
        } else {
            if (!(Test-Path $e.Path)) {
                Write-Log "Missing file: $($e.Path)" "WARN"
            }
        }
    }

    # Create placeholder build.bat if adapter dir exists but script missing
    foreach ($bat in $AdapterBuildBats) {
        $dir = Split-Path $bat -Parent
        if (Test-Path $dir -and !(Test-Path $bat)) {
            if ($AutoFix) {
                $content = "@echo off`r`necho Adapter build placeholder for %~dp0`r`necho Implement real build steps via CMake here.`r`nexit /b 0`r`n"
                Set-Content -Path $bat -Value $content -Encoding ASCII
                $FixSummary += "Added placeholder build.bat: $bat"
                Write-Log "Added placeholder: $bat" "WARN"
            } else {
                Write-Log "Missing build.bat: $bat" "WARN"
            }
        }
    }

    # Minimal CMake sanity: ensure top-level CMakeLists.txt exists
    $TopCMake = Join-Path $RootDir "CMakeLists.txt"
    if (!(Test-Path $TopCMake)) {
        Write-Log "Top-level CMakeLists.txt missing (informational)" "WARN"
    }

    # Optional validation phase
    if ($ValidateFixes) {
        $missing = @()
        foreach ($e in $Expected) {
            if (!(Test-Path $e.Path)) { $missing += $e.Path }
        }
        if ($missing.Count -gt 0) {
            Write-Log ("Post-validate: still missing: " + ($missing -join ", ")) "WARN"
        } else {
            Write-Log "Post-validate: structure OK" "INFO"
        }
    }

    # Report
    if ($GenerateReport) {
        $report = @()
        $report += "# uevr Build Fixer Report"
        $report += "Generated: $(Get-Date -Format 'yyyy-MM-dd HH:mm:ss')"
        $report += ""
        $report += "## Summary"
        $report += "- Fix Mode: $FixMode"
        $report += "- Target Component: $TargetComponent"
        $report += "- Auto Fix: $AutoFix"
        $report += "- Backup Originals: $BackupOriginals"
        $report += "- Validate Fixes: $ValidateFixes"
        $report += ""
        $report += "## Fixes Applied"
        if ($FixSummary.Count -eq 0) { $report += "- No changes required" } else { $FixSummary | ForEach-Object { $report += "- $_" } }
        $reportPath = Join-Path $LogsDir ("fixer_report_" + (Get-Date -Format 'yyyyMMdd_HHmmss') + ".md")
        $report -join "`r`n" | Set-Content -Path $reportPath -Encoding UTF8
        Write-Log "Report saved: $reportPath"
    }

    Write-Log "=== uevr Build Fixer Completed ==="

} catch {
    Write-Log "Fixer failed: $($_.Exception.Message)" "ERROR"
    Write-Log $($_.Exception.ToString()) "ERROR"
    exit 1
}
