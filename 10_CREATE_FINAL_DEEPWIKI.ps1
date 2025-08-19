# UEVR Final DeepWiki Creator
# Opretter den komplette deepwiki baseret på alle analyser

Write-Host "=== OPRETTER KOMPLET DEEPWIKI ===" -ForegroundColor Cyan

# Opret docs/deepwiki mappe hvis den ikke findes
$docsPath = "docs\deepwiki"
if (-not (Test-Path $docsPath)) {
    New-Item -ItemType Directory -Path $docsPath -Force | Out-Null
    Write-Host "✅ Oprettet mappe: $docsPath" -ForegroundColor Green
}

$outputFile = "$docsPath\DEEPWIKI_KOMPLET.md"

$markdown = @"
# 🌟 UEVR Complete DeepWiki
## Den dybe tallerken for alle projekters fulde filer

**Genereret:** $(Get-Date -Format "yyyy-MM-dd HH:mm:ss")  
**Status:** 🎯 KOMPLET - Alle funktioner dokumenteret

---

## 🎯 Formål

Dette dokument er den dybe tallerken (deep dish) for alle UEVR projekters fulde filer, så hver enkelt funktion er dokumenteret uden placeholders eller fejl.

---

## 📚 System Oversigt

### 🏗️ Arkitektur
UEVR (Universal Engine VR Adapter) er et cross-engine VR injection framework der understøtter:

- **MT Framework** (Capcom)
- **RE Engine** (Resident Evil)  
- **REDengine 4** (CD Projekt Red)
- **Unreal Engine** (Epic Games)
- **Unity Engine** (Unity Technologies)

### 🔧 Core Komponenter
- **Framework**: Hovedsystem og koordinering
- **Hooks**: Graphics, input og window message hooks
- **Mods**: Plugin system og VR integration
- **Adapters**: Engine-specifikke implementeringer

---

## 📁 Komplette Filer

"@

# Saml alle kildefiler
$sourceFiles = Get-ChildItem -Path "src" -Recurse -Include "*.cpp", "*.hpp", "*.h" | Where-Object { $_.FullName -notlike "*build*" -and $_.FullName -notlike "*deps*" -and $_.FullName -notlike "*thirdparty*" }

$totalFiles = $sourceFiles.Count
$totalLines = 0
$totalFunctions = 0

foreach ($file in $sourceFiles) {
    $content = Get-Content $file.FullName -Raw
    $lines = ($content -split "`n").Count
    $functions = ($content | Select-String "void.*\(.*\)").Count
    
    $totalLines += $lines
    $totalFunctions += $functions
    
    $fileName = Split-Path $file -Leaf
    $filePath = $file.FullName
    $relativePath = $file.FullName.Replace((Get-Location).Path, "").TrimStart("\")
    
    $markdown += @"

### 📄 $fileName
**Sti:** `$relativePath`  
**Størrelse:** $((Get-Item $file.FullName).Length) bytes  
**Linjer:** $lines  
**Funktioner:** $functions  
**Sidst ændret:** $((Get-Item $file.FullName).LastWriteTime.ToString("yyyy-MM-dd HH:mm:ss"))

#### 🔗 Komplet Indhold
```cpp
$content
```

---
"@
}

$markdown += @"

---

## 📊 Statistik

- **Total antal filer:** $totalFiles
- **Total antal linjer:** $totalLines
- **Total antal funktioner:** $totalFunctions
- **Gennemsnit funktioner per fil:** $([math]::Round($totalFunctions / $totalFiles, 2))

---

## 🎯 Implementerings Status

### ✅ Komplette Filer
Filer hvor alle funktioner er implementeret:

"@

# Find komplette filer
foreach ($file in $sourceFiles) {
    $headerFile = $file.FullName -replace "\.cpp$", ".hpp" -replace "\.h$", ".hpp"
    
    if (Test-Path $headerFile) {
        $headerContent = Get-Content $headerFile -Raw
        $headerFunctions = $headerContent | Select-String "void.*\(.*\);" | ForEach-Object { $_.Matches[0].Value.Trim() }
        
        $cppContent = Get-Content $file.FullName -Raw
        $implementedFunctions = $cppContent | Select-String "void.*::.*\(.*\)" | ForEach-Object { $_.Matches[0].Value.Trim() }
        
        $missingCount = 0
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
                $missingCount++
            }
        }
        
        if ($missingCount -eq 0) {
            $markdown += @"
- **$($file.Name)** - Alle funktioner implementeret
"@
        }
    }
}

$markdown += @"

### ⚠️ Filer der mangler implementering
Filer hvor nogle funktioner mangler implementering:

"@

# Find filer med manglende implementeringer
foreach ($file in $sourceFiles) {
    $headerFile = $file.FullName -replace "\.cpp$", ".hpp" -replace "\.h$", ".hpp"
    
    if (Test-Path $headerFile) {
        $headerContent = Get-Content $headerFile -Raw
        $headerFunctions = $headerContent | Select-String "void.*\(.*\);" | ForEach-Object { $_.Matches[0].Value.Trim() }
        
        $cppContent = Get-Content $file.FullName -Raw
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
            $markdown += @"
- **$($file.Name)** - $($missingFunctions.Count) manglende funktioner
"@
        }
    }
}

$markdown += @"

---

## 🚀 Næste Skridt

### 1. Prioriter implementeringer
Start med filer der har flest manglende funktioner

### 2. Brug deepwiki og context7
- Analyser eksisterende implementeringer
- Forstå funktionaliteten
- Implementer manglende funktioner

### 3. Test alle implementeringer
- Kompiler uden fejl
- Test funktionalitet
- Valider mod krav

---

## 📝 Noter

- Dette dokument er automatisk genereret
- Alle filer er inkluderet med komplet indhold
- Funktioner er analyseret og kategoriseret
- Implementerings status er opdateret

---

*UEVR DeepWiki - Den dybe tallerken for alle projekters fulde filer* 🍕
"@

$markdown | Out-File -FilePath $outputFile -Encoding UTF8
Write-Host "✅ Komplet deepwiki oprettet: $outputFile" -ForegroundColor Cyan
Write-Host "📊 Total filer: $totalFiles" -ForegroundColor Cyan
Write-Host "📊 Total linjer: $totalLines" -ForegroundColor Cyan
Write-Host "📊 Total funktioner: $totalFunctions" -ForegroundColor Cyan
