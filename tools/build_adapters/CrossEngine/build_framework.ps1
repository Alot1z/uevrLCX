#requires -Version 5.1
Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

. "$PSScriptRoot\..\_shared\build_common.ps1"

param(
  [ValidateSet('Release','Debug')][string]$Config = 'Release',
  [switch]$Clean
)

$repoRoot = Get-RepoRoot -ScriptRoot $PSScriptRoot
$src = Join-Path $repoRoot 'SOURCECODE\uevr'
$build = Join-Path $src 'build'
if ($Clean) { if (Test-Path $build) { Remove-Item -Recurse -Force $build } }

$gen = Get-Generator
$ts = New-BuildTimestamp
$logDir = Join-Path $repoRoot "logs\build\CrossEngine\$ts"
Ensure-Directory -Path $logDir
$configureLog = Join-Path $logDir 'configure.log'
$buildLog = Join-Path $logDir 'build.log'
$summary = Join-Path $logDir 'BuildSummary.md'

Ensure-Directory -Path $build

# Configure
$cxxFlags = '/W4 /WX /MP /permissive- /Zc:__cplusplus'
$cmakeArgs = @('-S', $src, '-B', $build, "-DCMAKE_BUILD_TYPE=$Config", "-DCMAKE_CXX_FLAGS=$cxxFlags")
if ($gen) { $cmakeArgs = @('-G', $gen, '-A', 'x64') + $cmakeArgs }

(& cmake @cmakeArgs *>&1 | Tee-Object -FilePath $configureLog) | Out-Null
if ($LASTEXITCODE -ne 0) { throw "CMake configure failed. See $configureLog" }

# Build
$buildArgs = @('--build', $build, '--config', $Config, '--parallel', [Environment]::ProcessorCount)
if ($gen -and $gen -like 'Visual Studio*') { $buildArgs += @('--', '/p:TreatWarningsAsErrors=true') }
(& cmake @buildArgs *>&1 | Tee-Object -FilePath $buildLog) | Out-Null
if ($LASTEXITCODE -ne 0) { throw "CMake build failed. See $buildLog" }

# Analyze logs — fail on ANY warnings
$r1 = Analyze-Log -LogPath $configureLog
$r2 = Analyze-Log -LogPath $buildLog
$warns = $r1.Warnings + $r2.Warnings
$errs = $r1.Errors + $r2.Errors

# Artifacts
$artifacts = Find-OutputDlls -SearchRoot $build

$genLabel = if ($gen) { $gen } else { 'Default' }
Write-BuildSummary -SummaryPath $summary -ProjectName 'uevr Main Framework' -Engine 'CrossEngine Core' -Config $Config -Generator $genLabel -BuildDir $build -ConfigureLog $configureLog -BuildLog $buildLog -WarnCount $warns -ErrCount $errs -Artifacts $artifacts

if ($errs -gt 0) { Write-Error "Errors detected in build. See $summary"; exit 1 }
if ($warns -gt 0) { Write-Error "Warnings detected (treated as errors). See $summary"; exit 2 }

Write-Host "Build succeeded with zero warnings." -ForegroundColor Green

