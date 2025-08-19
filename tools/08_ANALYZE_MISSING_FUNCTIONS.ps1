# UEVR Missing Functions Analyzer
# Identificerer manglende funktions implementeringer

Write-Host "=== ANALYSERER MANGLENDE FUNKTIONER ===" -ForegroundColor Green

# Skift til parent directory (root af projektet)
$projectRoot = Split-Path (Split-Path $PSScriptRoot -Parent) -Parent
Set-Location $projectRoot
Write-Host "Arbejder i: $projectRoot" -ForegroundColor Yellow

# Opret docs/deepwiki mappe hvis den ikke findes
$docsPath = Join-Path $projectRoot "docs\deepwiki"
if (-not (Test-Path $docsPath)) {
    New-Item -ItemType Directory -Path $docsPath -Force | Out-Null
    Write-Host "OK Oprettet mappe: $docsPath" -ForegroundColor Green
}

$outputFile = Join-Path $docsPath "MANGLENDE_FUNKTIONER.md"

$markdown = @"
# ❌ UEVR Missing Functions Analysis
## Identificerer manglende funktions implementeringer

**Genereret:** $(Get-Date -Format "yyyy-MM-dd HH:mm:ss")  
**Status:** 🔍 Analyserer...

---

## 📊 Oversigt

Dette dokument identificerer funktioner der er deklareret i header filer (.hpp, .h) men ikke implementeret i deres tilsvarende source filer (.cpp).

---

## 🔍 Analyseres...

"@

# Saml alle header filer
$headerFiles = Get-ChildItem -Path "src" -Recurse -Include "*.hpp", "*.h" | Where-Object { $_.FullName -notlike "*build*" -and $_.FullName -notlike "*deps*" -and $_.FullName -notlike "*thirdparty*" }

$totalHeaders = $headerFiles.Count
$filesWithMissing = 0
$totalMissing = 0

foreach ($headerFile in $headerFiles) {
    $cppFile = $headerFile.FullName -replace "\.hpp$", ".cpp" -replace "\.h$", ".cpp"
    
    if (Test-Path $cppFile) {
        $headerContent = Get-Content $headerFile.FullName -Raw
        $headerFunctions = $headerContent | Select-String "void.*\(.*\);" | ForEach-Object { $_.Matches[0].Value.Trim() }
        
        $cppContent = Get-Content $cppFile -Raw
        $implementedFunctions = $cppContent | Select-String "void.*::.*\(.*\)" | ForEach-Object { $_.Matches[0].Value.Trim() }
        
        $missingFunctions = @()
        foreach ($headerFunc in $headerFunctions) {
            $funcName = $headerFunc -replace "void\s+", "" -replace "\(.*\)", ""
            $found = $false
            
            foreach ($implFunc in $implementedFunctions) {
                if ($implFunc -like "*$funcName*") {
                    $found = $true
                    break
                }
            }
            
            if (-not $found) {
                $missingFunctions += $headerFunc
            }
        }
        
        if ($missingFunctions.Count -gt 0) {
            $filesWithMissing++
            $totalMissing += $missingFunctions.Count
            
            $markdown += @"

### 📁 $($headerFile.Name)
**Header:** `$($headerFile.FullName)`  
**Source:** `$cppFile`  
**Manglende funktioner:** $($missingFunctions.Count)

#### ❌ Manglende implementeringer:
"@
            
            foreach ($missingFunc in $missingFunctions) {
                $markdown += @"
```cpp
$missingFunc
```
"@
            }
        }
    }
}

$markdown += @"

---

## 📊 Statistik

- **Total header filer:** $totalHeaders
- **Filer med manglende funktioner:** $filesWithMissing
- **Total manglende funktioner:** $totalMissing

---

## 🎯 Anbefalinger

### Prioriter implementeringer efter:
1. **Antal manglende funktioner** - Start med filer der har flest
2. **Vigtighed** - Core system funktioner først
3. **Afhængigheder** - Implementer grundlæggende funktioner først

---

## 🚀 Næste skridt

1. **Analyser eksisterende implementeringer** for at forstå mønstre
2. **Implementer manglende funktioner** systematisk
3. **Test grundigt** efter hver implementering
4. **Kør denne analyse igen** for at se fremskridt

---

*Lad os få alle funktioner implementeret!* 💪
"@

$markdown | Out-File -FilePath $outputFile -Encoding UTF8
Write-Host "OK Manglende funktioner analyse gemt til: $outputFile" -ForegroundColor Green
Write-Host "INFO Total header filer: $totalHeaders" -ForegroundColor Cyan
Write-Host "INFO Filer med manglende funktioner: $filesWithMissing" -ForegroundColor Cyan
Write-Host "INFO Total manglende funktioner: $totalMissing" -ForegroundColor Cyan
