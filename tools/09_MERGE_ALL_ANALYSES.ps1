# UEVR Merge All Analyses
# Samler alle analyser i ét komplet dokument

Write-Host "=== SAMLER ALLE ANALYSER ===" -ForegroundColor Magenta

# Opret docs/deepwiki mappe hvis den ikke findes
$docsPath = "docs\deepwiki"
if (-not (Test-Path $docsPath)) {
    New-Item -ItemType Directory -Path $docsPath -Force | Out-Null
    Write-Host "✅ Oprettet mappe: $docsPath" -ForegroundColor Green
}

$outputFile = "$docsPath\ALLE_ANALYSER_SAMLET.md"
$analysisFiles = @(
    "$docsPath\CORE_ANALYSE.md",
    "$docsPath\HOOKS_ANALYSE.md", 
    "$docsPath\MODS_ANALYSE.md",
    "$docsPath\MANGLENDE_FUNKTIONER.md"
)
