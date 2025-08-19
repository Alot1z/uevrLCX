# UEVR Core Files Analyzer
# Analyserer alle core filer og formaterer dem i perfekt markdown

Write-Host "=== ANALYSERER CORE FILER ===" -ForegroundColor Green

# Opret docs/deepwiki mappe hvis den ikke findes
$docsPath = "docs\deepwiki"
if (-not (Test-Path $docsPath)) {
    New-Item -ItemType Directory -Path $docsPath -Force | Out-Null
    Write-Host "✅ Oprettet mappe: $docsPath" -ForegroundColor Green
}

$outputFile = "$docsPath\CORE_ANALYSE.md"
$coreFiles = @(
    "src\core\Framework.cpp",
    "src\core\Framework.hpp", 
    "src\core\Main.cpp",
    "src\core\Mod.cpp",
    "src\core\Mod.hpp",
    "src\core\Mods.cpp",
    "src\core\Mods.hpp",
    "src\core\WindowFilter.cpp",
    "src\core\WindowFilter.hpp",
    "src\core\ExceptionHandler.cpp",
    "src\core\ExceptionHandler.hpp"
)

$markdown = @"
# 🔧 UEVR Core Files Analysis
## Komplet analyse af alle core filer

**Genereret:** $(Get-Date -Format "yyyy-MM-dd HH:mm:ss")  
**Antal filer:** $($coreFiles.Count)  
**Status:** ✅ Komplet

---

"@

foreach ($file in $coreFiles) {
    if (Test-Path $file) {
        Write-Host "Analyserer: $file" -ForegroundColor Yellow
        
        $content = Get-Content $file -Raw
        $fileName = Split-Path $file -Leaf
        $filePath = $file
        
        # Analyser funktioner
        $functions = $content | Select-String "void.*\(.*\)" | ForEach-Object { $_.Matches[0].Value.Trim() }
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

### ⚙️ Functions
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
Write-Host "✅ Core analyse gemt til: $outputFile" -ForegroundColor Green
