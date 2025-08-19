#requires -Version 5.1
Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

function New-BuildTimestamp {
    (Get-Date).ToString('yyyyMMdd-HHmmss')
}

function Ensure-Directory {
    param([Parameter(Mandatory)][string]$Path)
    if (-not (Test-Path -LiteralPath $Path)) { New-Item -ItemType Directory -Path $Path | Out-Null }
}

function Get-RepoRoot {
    # scripts live in tools/build_adapters/<engine>/; repo root is two levels up from tools
    param([Parameter(Mandatory)][string]$ScriptRoot)
    $toolsDir = (Resolve-Path (Join-Path $ScriptRoot '..')).Path
    $repoRoot = (Resolve-Path (Join-Path $toolsDir '..')).Path
    return $repoRoot
}

function Get-Generator {
    # Detect supported Visual Studio generator from cmake --help
    try {
        $help = & cmake --help 2>&1 | Out-String
    } catch {
        return $null
    }
    if ($LASTEXITCODE -ne 0) { return $null }
    if ($help -match 'Visual Studio 17 2022') { return 'Visual Studio 17 2022' }
    if ($help -match 'Visual Studio 16 2019') { return 'Visual Studio 16 2019' }
    return $null
}

function Invoke-CMakeConfigure {
    param(
        [Parameter(Mandatory)][string]$SourceDir,
        [Parameter(Mandatory)][string]$BuildDir,
        [ValidateSet('Release','Debug')][string]$Config = 'Release',
        [string]$Generator,
        [string]$Arch = 'x64',
        [string]$CxxFlags = '/W4 /WX /MP /permissive- /Zc:__cplusplus',
        [hashtable]$Definitions
    )

    Ensure-Directory -Path $BuildDir
    $cmakeArgs = @('-S', $SourceDir, '-B', $BuildDir, "-DCMAKE_BUILD_TYPE=$Config", "-DCMAKE_CXX_FLAGS=$CxxFlags")
    if ($Generator) { $cmakeArgs = @('-G', $Generator, '-A', $Arch) + $cmakeArgs }
    if ($Definitions) {
        foreach ($k in $Definitions.Keys) { $cmakeArgs += "-D$k=$($Definitions[$k])" }
    }
    & cmake @cmakeArgs
    return $LASTEXITCODE
}

function Invoke-CMakeBuild {
    param(
        [Parameter(Mandatory)][string]$BuildDir,
        [ValidateSet('Release','Debug')][string]$Config = 'Release',
        [int]$Parallel = [Environment]::ProcessorCount,
        [string]$Generator
    )
    $buildArgs = @('--build', $BuildDir, '--config', $Config, '--parallel', $Parallel)
    # If using a VS generator, pass TreatWarningsAsErrors to MSBuild for extra safety
    if ($Generator -and $Generator -like 'Visual Studio*') {
        $buildArgs += @('--', '/p:TreatWarningsAsErrors=true')
    }
    & cmake @buildArgs
    return $LASTEXITCODE
}

function Write-BuildSummary {
    param(
        [Parameter(Mandatory)][string]$SummaryPath,
        [Parameter(Mandatory)][string]$ProjectName,
        [Parameter(Mandatory)][string]$Engine,
        [Parameter(Mandatory)][string]$Config,
        [Parameter(Mandatory)][string]$Generator,
        [Parameter(Mandatory)][string]$BuildDir,
        [Parameter(Mandatory)][string]$ConfigureLog,
        [Parameter(Mandatory)][string]$BuildLog,
        [Parameter(Mandatory)][int]$WarnCount,
        [Parameter(Mandatory)][int]$ErrCount,
        [string[]]$Artifacts,
        [hashtable]$Extra
    )
    $ts = Get-Date -Format 'yyyy-MM-dd HH:mm:ss'
    $artLines = @()
    foreach ($a in ($Artifacts | Sort-Object -Unique)) { $artLines += "- $a" }
    $adv = @()
    if ($WarnCount -gt 0) { $adv += "Fix all warnings (treated as errors). Consider suppressing only well-justified, documented cases." }
    if ($ErrCount -gt 0) { $adv += "Resolve compilation/link errors. Inspect $BuildLog for first failure location." }
    if (-not $adv) { $adv = @('No issues detected. Consider enabling static analysis (clang-tidy / MSVC Code Analysis) for deeper checks.') }

    $content = @(
        "# Build Summary — $ProjectName",
        "- Timestamp: $ts",
        "- Engine: $Engine",
        "- Configuration: $Config",
        "- Generator: $Generator",
        "- Build Dir: $BuildDir",
        "- Configure Log: $ConfigureLog",
        "- Build Log: $BuildLog",
        "- Warnings: $WarnCount",
        "- Errors: $ErrCount",
        "",
        "## Artifacts",
        ($artLines -join "`n"),
        "",
        "## Advisory",
        ($adv -join "`n- ") -replace '^(?=.)','- ', 1
    ) -join "`n"

    $content | Out-File -FilePath $SummaryPath -Encoding UTF8 -Force
}

function Analyze-Log {
    param([Parameter(Mandatory)][string]$LogPath)
    $text = Get-Content -LiteralPath $LogPath -Raw
    # Count case-insensitive occurrences of 'warning' excluding final MSBuild summary line with '0 Warning(s)'
    $warn = ([regex]::Matches($text, '(?i)\bwarning\b')).Count
    $err = ([regex]::Matches($text, '(?i)\berror\b')).Count
    # Reduce false positives from summary lines
    $summWarnZero = ([regex]::Matches($text, '(?im)^\s*0 Warning\(s\)\s*$')).Count
    if ($warn -gt 0 -and $summWarnZero -gt 0) { $warn = 0 }
    return [pscustomobject]@{ Warnings = $warn; Errors = $err }
}

function Find-OutputDlls {
    param([Parameter(Mandatory)][string]$SearchRoot)
    if (-not (Test-Path -LiteralPath $SearchRoot)) { return @() }
    Get-ChildItem -LiteralPath $SearchRoot -Recurse -Include *.dll -ErrorAction SilentlyContinue |
        Select-Object -ExpandProperty FullName
}

Export-ModuleMember -Function *
