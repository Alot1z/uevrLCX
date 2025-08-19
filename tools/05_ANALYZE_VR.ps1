# UEVR VR Files Analyzer
# Analyserer alle VR filer og formaterer dem i perfekt markdown

Write-Host "=== ANALYSERER VR FILER ===" -ForegroundColor Green

# Opret docs/deepwiki mappe hvis den ikke findes
$docsPath = "docs\deepwiki"
if (-not (Test-Path $docsPath)) {
    New-Item -ItemType Directory -Path $docsPath -Force | Out-Null
    Write-Host "✅ Oprettet mappe: $docsPath" -ForegroundColor Green
}

$outputFile = "$docsPath\VR_ANALYSE.md"
$vrFiles = @(
    "src\vr\OpenXR.cpp",
    "src\vr\OpenXR.hpp",
    "src\vr\OpenVR.cpp",
    "src\vr\OpenVR.hpp",
    "src\vr\VRManager.cpp",
    "src\vr\VRManager.hpp",
    "src\vr\VRRenderer.cpp",
    "src\vr\VRRenderer.hpp"
)

$markdown = @"
# 🥽 UEVR VR Files Analysis
## Komplet analyse af alle VR filer

**Genereret:** $(Get-Date -Format "yyyy-MM-dd HH:mm:ss")  
**Antal filer:** $($vrFiles.Count)  
**Status:** ✅ Komplet

---

"@

foreach ($file in $vrFiles) {
    if (Test-Path $file) {
        Write-Host "Analyserer: $file" -ForegroundColor Yellow
        
        $content = Get-Content $file -Raw
        $fileName = Split-Path $file -Leaf
        $filePath = $file
        
        # Analyser funktioner og VR
        $functions = $content | Select-String "void.*\(.*\)" | ForEach-Object { $_.Matches[0].Value.Trim() }
        $vrFunctions = $content | Select-String ".*VR.*\(.*\)" | ForEach-Object { $_.Matches[0].Value.Trim() }
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

### ⚙️ VR Functions
```cpp
$($vrFunctions -join "`n")
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
Write-Host "✅ VR analyse gemt til: $outputFile" -ForegroundColor Green
