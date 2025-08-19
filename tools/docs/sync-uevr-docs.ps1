param(
    [string]$DocsRoot = "docs/uevr-docs/src"
)

$ErrorActionPreference = 'Stop'

function Get-MarkdownFiles([string]$Root) {
    if (-not (Test-Path $Root)) {
        throw "Docs root not found: $Root"
    }
    Get-ChildItem -Path $Root -Recurse -File -Include *.md
}

function Resolve-LinkTarget([string]$Root, [string]$CurrentFile, [string]$Link) {
    # Normalize anchors and query fragments
    $clean = $Link.Split('#')[0]
    if ([string]::IsNullOrWhiteSpace($clean)) { return $null }
    # Absolute http(s) links are ignored (external)
    if ($clean -match '^[a-z]+://') { return $null }
    # Handle absolute-from-root style paths starting with '/'
    if ($clean.StartsWith('/')) {
        return Join-Path $Root $clean.TrimStart('/')
    }
    # Relative to current file
    $base = Split-Path -Path $CurrentFile -Parent
    return Join-Path $base $clean
}

function Find-LocalBrokenLinks([string]$Root) {
    $mdFiles = Get-MarkdownFiles -Root $Root
    $broken = @()
    foreach ($file in $mdFiles) {
        $content = Get-Content -LiteralPath $file.FullName -Raw
        # Match markdown links: [text](target)
        $matches = [regex]::Matches($content, "\[[^\]]+\]\(([^)]+)\)")
        foreach ($m in $matches) {
            $target = $m.Groups[1].Value.Trim()
            $resolved = Resolve-LinkTarget -Root $Root -CurrentFile $file.FullName -Link $target
            if ($null -ne $resolved) {
                # Only check local filesystem targets ending with .md or assets
                if ($resolved -match "\.md$" -or $resolved -match "\.(png|jpg|jpeg|svg|gif)$") {
                    if (-not (Test-Path -LiteralPath $resolved)) {
                        $broken += [pscustomobject]@{
                            File   = $file.FullName.Replace($Root, '').TrimStart('\\','/')
                            Link   = $target
                            Expect = $resolved.Replace($Root, '').TrimStart('\\','/')
                        }
                    }
                }
            }
        }
    }
    return $broken
}

Write-Host "Validating local documentation (no network) ..." -ForegroundColor Green
$Root = (Resolve-Path $DocsRoot).Path
$files = Get-MarkdownFiles -Root $Root
Write-Host ("Found {0} markdown files under {1}" -f $files.Count, $Root) -ForegroundColor Cyan

$broken = Find-LocalBrokenLinks -Root $Root
if ($broken.Count -eq 0) {
    Write-Host "No broken local links found." -ForegroundColor Green
} else {
    Write-Warning ("Found {0} broken local links:" -f $broken.Count)
    $broken | Sort-Object File, Link | Format-Table -AutoSize | Out-Host
    exit 2
}

Write-Host "Docs validation complete." -ForegroundColor Green
