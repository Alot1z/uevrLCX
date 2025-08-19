[CmdletBinding()]
param(
  [string]$BaseSha = "",
  [string]$HeadSha = "",
  [string]$EventName = ""
)

function Get-ChangedFiles {
  param(
    [string]$Base,
    [string]$Head
  )
  $range = "$Base..$Head"
  Write-Host "Diff range: $range"
  $out = git diff --name-only $range | Out-String
  return @($out.Trim().Split("`n") | Where-Object { $_ -and ($_ -ne '') })
}

if (-not $HeadSha) {
  $HeadSha = (git rev-parse HEAD).Trim()
}
if (-not $BaseSha) {
  if ($EventName -eq 'push') {
    $BaseSha = (git rev-parse "$HeadSha^").Trim()
  } else {
    try {
      $BaseSha = (git merge-base origin/HEAD $HeadSha).Trim()
    } catch {
      # Fallback: previous commit
      $BaseSha = (git rev-parse "$HeadSha^").Trim()
    }
  }
}

Write-Host "Never-Stop Guard | Event=$EventName Base=$BaseSha Head=$HeadSha"

$changed = Get-ChangedFiles -Base $BaseSha -Head $HeadSha
if ($changed.Count -eq 0) {
  Write-Host "No changed files detected."
}

# Forbidden terms (case-insensitive), designed to catch AI/tool traces without false positives
$forbidden = @(
  'chatgpt',
  'gpt',
  'openai',
  'anthropic',
  'claude',
  'copilot',
  'codewhisperer',
  'cursor',
  'tabnine',
  'deepseek',
  'llm',
  'ai-generated',
  'ai generated',
  'vibecode',
  'bard',
  'gemini',
  'replit',
  'codepilot',
  'windsurf'
)

$violations = @()

# Scan file contents (text-like extensions)
$textExtensions = @('.c','.cc','.cpp','.cxx','.h','.hpp','.hh','.hxx','.cs','.py','.js','.ts','.tsx','.java','.md','.cmake','.toml','.yml','.yaml','.json','.ps1','.bat','.sh','.lua','.txt','.ini','.cfg','.cppm')
$excludeFiles = @('tools/ci/never_stop_guard.ps1', '.github/workflows/never-stop-guard.yml')
foreach ($f in $changed) {
  if (-not (Test-Path $f)) { continue }
  $rel = $f.Replace('\\','/').ToLowerInvariant()
  if ($excludeFiles -contains $rel) { continue }
  $ext = [System.IO.Path]::GetExtension($f).ToLowerInvariant()
  if ($textExtensions -contains $ext) {
    try {
      $content = Get-Content -Path $f -Raw -ErrorAction Stop
      foreach ($pat in $forbidden) {
        if ($content -match [Regex]::Escape($pat)) {
          $violations += "Forbidden term '$pat' found in file: $f"
        }
      }
    } catch {
      Write-Host "Skipping non-text or unreadable file: $f"
    }
  }
}

# Scan commit messages
$commitLog = (git log --pretty=%B "$BaseSha..$HeadSha") | Out-String
foreach ($pat in $forbidden) {
  if ($commitLog -match [Regex]::Escape($pat)) {
    $violations += "Forbidden term '$pat' found in commit messages"
  }
}

# Research log requirement: if code changed, require at least one docs/contributing/logs/*.md in the diff
function Test-CodeChange([string]$path) {
  $p = $path.Replace('\\','/').ToLowerInvariant()
  # Restrict to code directories only; docs/config-only changes won't trigger research log
  if (
    $p.StartsWith('src/') -or
    $p.StartsWith('include/') -or
    $p.StartsWith('adapters/') -or
    $p.StartsWith('lua-api/') -or
    $p.StartsWith('sourcecode/') -or
    $p.StartsWith('uevr/')
  ) { return $true }
  return $false
}

$codeChanged = $false
$logProvided = $false
foreach ($f in $changed) {
  if (Test-CodeChange $f) { $codeChanged = $true }
  $pl = $f.ToLowerInvariant()
  if ($pl.StartsWith('docs/contributing/logs/') -and $pl.EndsWith('.md')) { $logProvided = $true }
}

if ($codeChanged -and -not $logProvided) {
  $violations += "Research log required: add docs/contributing/logs/<yyyy-mm-dd>-<topic>.md documenting sources and decisions."
}

if ($violations.Count -gt 0) {
  Write-Error ("Never-Stop Guard violations:`n" + ($violations -join "`n"))
  exit 1
}

Write-Host "Never-Stop Guard passed."
