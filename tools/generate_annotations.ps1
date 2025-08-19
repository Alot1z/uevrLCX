param(
  [string]$TargetRoot
)

# Resolve root (repo root is parent of this script's directory)
$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$Root = Split-Path -Parent $ScriptDir
if ($TargetRoot) {
  $Root = (Resolve-Path $TargetRoot).Path
}
$OutDir = Join-Path $Root "docs\annotations"

# Mapping rules: path substring -> (context7 library id, deepwiki page)
$RULES = @(
  @{ match="\src\openxr\"; ctx="/khronosgroup/openxr-docs"; wiki="3.2 VR Runtime Implementations" },
  @{ match="\src\openvr\"; ctx="/valvesoftware/openvr"; wiki="3.2 VR Runtime Implementations" },
  @{ match="\src\d3d11\"; ctx="/microsoft/directx-graphics-samples"; wiki="3.1 D3D11 and D3D12 Components" },
  @{ match="\src\d3d12\"; ctx="/microsoft/directx-graphics-samples"; wiki="3.1 D3D11 and D3D12 Components" },
  @{ match="\include\uevr\"; ctx="/praydog/uevr"; wiki="4 Plugin and Extension System" },
  @{ match="\src\plugin\"; ctx="/praydog/uevr"; wiki="4.1 Plugin Loader" },
  @{ match="\lua-api\"; ctx="/praydog/uevr"; wiki="4.2 Lua Scripting" },
  @{ match="\docs\uevr-docs\"; ctx="/praydog/uevr"; wiki="5 Configuration System" },
  @{ match="\SOURCECODE\"; ctx="/praydog/uevr"; wiki="1.1 Architecture Overview" },
  @{ match="\src\sdk\"; ctx="/mrrobinofficial/guide-unrealengine"; wiki="Architecture" }
)
$DEFAULT_CTX = @("/praydog/uevr","1.1 Architecture Overview")

# Text extensions
$TextExts = @(
  ".c", ".cc", ".cpp", ".cxx", ".h", ".hpp", ".hxx", ".inl", ".ipp",
  ".cs", ".py", ".lua", ".md", ".txt", ".cmake", ".json", ".toml", ".ini", ".cfg",
  ".bat", ".sh", ".ps1"
)

$SkipDirs = @(".git", ".qodo", ".vs", "build", "bin", "obj", "Binaries", "Intermediate")
$SkipPrefixes = @((Join-Path $OutDir ""))

function Is-UnderSkipDir([string]$path) {
  foreach ($d in $SkipDirs) {
    if ($path -like "*\$d\*") { return $true }
  }
  return $false
}

function Classify([string]$path) {
  foreach ($pfx in $SkipPrefixes) {
    if ($path.StartsWith($pfx, [System.StringComparison]::OrdinalIgnoreCase)) {
      return $null
    }
  }
  foreach ($r in $RULES) {
    if ($path -like ("*" + $r.match + "*")) { return @($r.ctx, $r.wiki) }
  }
  return $DEFAULT_CTX
}

function Is-TextFile([System.IO.FileInfo]$file) {
  $ext = [System.IO.Path]::GetExtension($file.FullName).ToLowerInvariant()
  if ($TextExts -contains $ext) { return $true }
  if ([string]::IsNullOrEmpty($ext)) {
    try {
      $fs = [System.IO.File]::Open($file.FullName, 'Open', 'Read', 'ReadWrite')
      try {
        $buf = New-Object byte[] 2048
        $read = $fs.Read($buf, 0, 2048)
      } finally { $fs.Close() }
      if ($buf -contains 0) { return $false }
      return $true
    } catch { return $false }
  }
  return $false
}

function Annotate-File([System.IO.FileInfo]$file) {
  $cls = Classify $file.FullName
  if ($null -eq $cls) { return $null }
  $ctx7 = $cls[0]
  $wiki = $cls[1]

  try {
    $lines = Get-Content -LiteralPath $file.FullName -Raw -Encoding UTF8 | Out-String
  } catch {
    $lines = Get-Content -LiteralPath $file.FullName -Raw -Encoding Default | Out-String
  }
  $arr = $lines -split "`r?`n"

  $annotations = @{}
  for ($i=0; $i -lt $arr.Count; $i++) {
    $lineNo = ($i+1).ToString()
    $annotations[$lineNo] = @(
      @{ source = "deepwiki"; repo = "praydog/uevr"; page = $wiki },
      @{ source = "context7"; library = $ctx7 }
    )
  }

  $rel = Resolve-Path -LiteralPath $file.FullName | ForEach-Object { $_.Path.Replace($Root+"\", "") }
  $outPath = Join-Path $OutDir ($rel + ".json")
  $outDirLocal = Split-Path -Parent $outPath
  New-Item -ItemType Directory -Force -Path $outDirLocal | Out-Null

  $payload = [ordered]@{
    file = $file.FullName
    generated_at = (Get-Date).ToString("o")
    schema_version = 1
    annotations = $annotations
  }
  ($payload | ConvertTo-Json -Depth 8) | Out-File -LiteralPath $outPath -Encoding UTF8
  return $outPath
}

Write-Host "Annotating repo under: $Root"
New-Item -ItemType Directory -Force -Path $OutDir | Out-Null

$written = 0
Get-ChildItem -Path $Root -Recurse -File | ForEach-Object {
  $p = $_.FullName
  if ($p.StartsWith($OutDir, [System.StringComparison]::OrdinalIgnoreCase)) { return }
  if (Is-UnderSkipDir $p) { return }
  if (-not (Is-TextFile $_)) { return }
  try {
    $out = Annotate-File $_
    if ($out) { $written++ }
  } catch {
    Write-Warning "Failed to annotate $p: $_"
  }
}

Write-Host "Done. Wrote $written annotation files under $OutDir."
