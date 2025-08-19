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

$markdown = @"
# 🚀 UEVR Complete System Analysis
## Komplet samling af alle analyser

**Genereret:** $(Get-Date -Format "yyyy-MM-dd HH:mm:ss")  
**Antal analyser:** $($analysisFiles.Count)  
**Status:** ✅ Komplet samling

---

## 📋 Indholdsfortegnelse

"@

# Tilføj indholdsfortegnelse
foreach ($file in $analysisFiles) {
    if (Test-Path $file) {
        $fileName = $file -replace "\.md$", ""
        $markdown += @"
- [$fileName](#$($fileName.ToLower() -replace '[^a-z0-9]', '-'))
"@
    }
}

$markdown += @"

---

"@

# Saml alle analyser
foreach ($file in $analysisFiles) {
    if (Test-Path $file) {
        Write-Host "Samler: $file" -ForegroundColor Yellow
        
        $content = Get-Content $file -Raw
        $fileName = $file -replace "\.md$", ""
        
        $markdown += @"

## $fileName

$content

---
"@
    }
}

$markdown += @"

---

## 🎯 Næste skridt

1. **Gennemgå alle analyser**
2. **Identificer prioriterede filer**
3. **Implementer manglende funktioner**
4. **Brug deepwiki og context7 til forståelse**
5. **Test alle implementeringer**

---

*Dette dokument indeholder alle analyser samlet i ét sted*
"@

$markdown | Out-File -FilePath $outputFile -Encoding UTF8
Write-Host "✅ Alle analyser samlet i: $outputFile" -ForegroundColor Magenta
