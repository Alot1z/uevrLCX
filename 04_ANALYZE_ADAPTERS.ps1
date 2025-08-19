# UEVR Adapters Files Analyzer
# Analyserer alle adapter filer og formaterer dem i perfekt markdown

Write-Host "=== ANALYSERER ADAPTERS FILER ===" -ForegroundColor Green

# Opret docs/deepwiki mappe hvis den ikke findes
$docsPath = "docs\deepwiki"
if (-not (Test-Path $docsPath)) {
    New-Item -ItemType Directory -Path $docsPath -Force | Out-Null
    Write-Host "✅ Oprettet mappe: $docsPath" -ForegroundColor Green
}

$outputFile = "$docsPath\ADAPTERS_ANALYSE.md"
$adapterFiles = @(
    "src\adapters\redengine4\redengine_adapter.cpp",
    "src\adapters\redengine4\redengine_adapter.hpp",
    "src\adapters\redengine4\redengine4_adapter.cpp",
    "src\adapters\redengine4\redengine4_adapter.hpp",
    "src\adapters\redengine4\REDengine4Adapter.cpp",
    "src\adapters\redengine4\REDengine4Adapter.hpp",
    "src\adapters\redengine4\uobject_hook.cpp",
    "src\adapters\redengine4\uobject_hook.hpp"
)

$markdown = @"
# 🔌 UEVR Adapters Files Analysis
## Komplet analyse af alle adapter filer

**Genereret:** $(Get-Date -Format "yyyy-MM-dd HH:mm:ss")  
**Antal filer:** $($adapterFiles.Count)  
**Status:** ✅ Komplet

---

"@

foreach ($file in $adapterFiles) {
    if (Test-Path $file) {
        Write-Host "Analyserer: $file" -ForegroundColor Yellow
        
        $content = Get-Content $file -Raw
        $fileName = Split-Path $file -Leaf
        $filePath = $file
        
        # Analyser funktioner og adapters
        $functions = $content | Select-String "void.*\(.*\)" | ForEach-Object { $_.Matches[0].Value.Trim() }
        $adapterFunctions = $content | Select-String ".*Adapter.*\(.*\)" | ForEach-Object { $_.Matches[0].Value.Trim() }
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

### ⚙️ Adapter Functions
```cpp
$($adapterFunctions -join "`n")
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
Write-Host "✅ Adapters analyse gemt til: $outputFile" -ForegroundColor Green
