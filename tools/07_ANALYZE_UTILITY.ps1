# UEVR Utility Files Analyzer
# Analyserer alle utility filer og formaterer dem i perfekt markdown

Write-Host "=== ANALYSERER UTILITY FILER ===" -ForegroundColor Green

# Opret docs/deepwiki/analyses mappe hvis den ikke findes
$analysesPath = "docs\deepwiki\analyses"
if (-not (Test-Path $analysesPath)) {
    New-Item -ItemType Directory -Path $analysesPath -Force | Out-Null
    Write-Host "OK Oprettet mappe: $analysesPath" -ForegroundColor Green
}

$outputFile = "$analysesPath\UTILITY_ANALYSE.md"
$utilityFiles = @(
    "src\utility\Config.cpp",
    "src\utility\Config.hpp",
    "src\utility\FileUtils.cpp",
    "src\utility\FileUtils.hpp",
    "src\utility\Logger.cpp",
    "src\utility\Logger.hpp",
    "src\utility\StringUtils.cpp",
    "src\utility\StringUtils.hpp"
)

$markdown = @"
# 🛠️ UEVR Utility Files Analysis
## Komplet analyse af alle utility filer

**Genereret:** $(Get-Date -Format "yyyy-MM-dd HH:mm:ss")  
**Antal filer:** $($utilityFiles.Count)  
**Status:** ✅ Komplet

---

"@

foreach ($file in $utilityFiles) {
    if (Test-Path $file) {
        Write-Host "Analyserer: $file" -ForegroundColor Yellow
        
        $content = Get-Content $file -Raw
        $fileName = Split-Path $file -Leaf
        $filePath = $file
        
        # Analyser funktioner og utility
        $functions = $content | Select-String "void.*\(.*\)" | ForEach-Object { $_.Matches[0].Value.Trim() }
        $utilityFunctions = $content | Select-String ".*Utils.*\(.*\)" | ForEach-Object { $_.Matches[0].Value.Trim() }
        $classes = $content | Select-String "class.*\{" | ForEach-Object { $_.Matches[0].Value.Trim() }
        $includes = $content | Select-String "#include.*" | ForEach-Object { $_.Matches[0].Value.Trim() }
        
        $markdown += @"

## 📁 $fileName
**Sti:** `$filePath`  
**Størrelse:** $((Get-Item $file).Length) bytes  
**Sidst ændret:** $((Get-Item $file).LastWriteTime.ToString("yyyy-MM-dd HH:mm:ss"))

### 🔗 Includes
```cpp
$($includes -join "`n")
```

### 🏗️ Classes
```cpp
$($classes -join "`n")
```

### ⚙️ Utility Functions
```cpp
$($utilityFunctions -join "`n")
```

### ⚙️ General Functions
```cpp
$($functions -join "`n")
```

### 📄 Komplet Indhold
```cpp
$content
```

---
"@
    }
}

$markdown | Out-File -FilePath $outputFile -Encoding UTF8
Write-Host "✅ Utility analyse gemt til: $outputFile" -ForegroundColor Green
